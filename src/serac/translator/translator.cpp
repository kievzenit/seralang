#include "translator.h"

std::unique_ptr<llvm::Module> translator::translator::translate() {
    using namespace llvm;

    create_types();

    declare_functions();

    translate_global_vars();
    translate_function_declarations();

    return std::move(module_);
}

void translator::translator::create_types() {
    create_basic_types();
}

void translator::translator::create_basic_types() {
    using namespace llvm;
    using namespace emitter::ir;

    types_[type::boolean()->name] = Type::getInt1Ty(*context_);
    types_[integer_type::int1()->name] = Type::getInt1Ty(*context_);
    types_[integer_type::int8()->name] = Type::getInt8Ty(*context_);
    types_[integer_type::int16()->name] = Type::getInt16Ty(*context_);
    types_[integer_type::int32()->name] = Type::getInt32Ty(*context_);
    types_[integer_type::int64()->name] = Type::getInt64Ty(*context_);
    types_[integer_type::uint8()->name] = Type::getInt8Ty(*context_);
    types_[integer_type::uint16()->name] = Type::getInt16Ty(*context_);
    types_[integer_type::uint32()->name] = Type::getInt32Ty(*context_);
    types_[integer_type::uint64()->name] = Type::getInt64Ty(*context_);
}

void translator::translator::declare_functions() {
    using namespace llvm;

    for (auto &func_decl : package_ir_->func_declarations) {
        std::vector<Type*> params;
        for (const auto& param : func_decl->params) {
            params.push_back(types_[param.param_type->name]);
        }

        auto func_type = FunctionType::get(
                types_[func_decl->return_type->name],
                ArrayRef<Type*>(params),
                false);

        Function::Create(
                func_type,
                GlobalValue::LinkageTypes::ExternalLinkage,
                func_decl->name,
                *module_);
    }
}

void translator::translator::translate_global_vars() {
    using namespace llvm;

    auto init_globals_func_type = FunctionType::get(Type::getVoidTy(*context_), false);
    auto init_globals_func = Function::Create(
            init_globals_func_type,
            GlobalValue::LinkageTypes::PrivateLinkage,
            "init_globals",
            *module_);

    init_globals_func->setSection(".text.startup");

    auto llvm_global_ctors_element_type = StructType::get(
            *context_,
            {
                Type::getInt32Ty(*context_),
                PointerType::get(Type::getInt64Ty(*context_), 0),
                PointerType::get(Type::getInt64Ty(*context_), 0)
            });
    auto llvm_global_ctors_type = ArrayType::get(llvm_global_ctors_element_type, 1);

    auto llvm_global_ctors_initializer = ConstantArray::get(
            llvm_global_ctors_type,
            {
                ConstantStruct::get(
                    llvm_global_ctors_element_type,
                    {
                            ConstantInt::get(Type::getInt32Ty(*context_), 65535),
                            init_globals_func,
                            Constant::getNullValue(
                                PointerType::get(
                                        Type::getInt64Ty(*context_),
                                        0))
                    })});

    auto llvm_global_ctors = new GlobalVariable(
            *module_,
            llvm_global_ctors_type,
            false,
            GlobalValue::LinkageTypes::AppendingLinkage,
            llvm_global_ctors_initializer,
            StringRef("llvm.global_ctors"));

    auto generate_br = false;
    for (auto &global_var : package_ir_->global_variables) {
        translate_global_var(std::move(global_var), generate_br);
    }

    auto exit_block = BasicBlock::Create(
            *context_,
            "exit",
            init_globals_func);

    if (generate_br) {
        builder_->CreateBr(exit_block);
    }

    builder_->SetInsertPoint(exit_block);
    builder_->CreateRetVoid();
    builder_->ClearInsertionPoint();
}

void translator::translator::translate_global_var(
        std::unique_ptr<emitter::ir::let_stmt_ir> let_stmt,
        bool &generate_br) {
    using namespace llvm;

    auto var_type = types_[let_stmt->variable_type->name];

    auto initializer = let_stmt->expr->is_const_expr ?
                       (Constant*)translate_expr(let_stmt->expr.get()) :
                       ConstantInt::get(var_type, 0);

    auto variable = new GlobalVariable(
            *module_,
            var_type,
            false,
            let_stmt->is_private ?
            GlobalValue::LinkageTypes::InternalLinkage : GlobalValue::LinkageTypes::ExternalLinkage,
            initializer,
            let_stmt->name);

    variable->setAlignment(Align(let_stmt->variable_type->size)); // TODO: redo this

    if (let_stmt->expr->is_const_expr) {
        return;
    }

    auto init_globals_func = module_->getFunction("init_globals");
    auto init_block = BasicBlock::Create(
            *context_,
            let_stmt->name + "_init",
            init_globals_func);

    if (generate_br) {
        builder_->CreateBr(init_block);
    }

    builder_->SetInsertPoint(init_block);

    generate_br = true;
    auto result = translate_expr(let_stmt->expr.get());
    builder_->CreateStore(result, variable);
}

void translator::translator::translate_function_declarations() {
    bool main_translated = false;
    for (auto &func_decl : package_ir_->func_declarations) {
        if (func_decl->name == "main") {
            translate_main_function(std::move(func_decl));
            main_translated = true;
            continue;
        }

        if (func_decl->is_extern) {
            continue;
        }

        translate_function(std::move(func_decl));
    }

    if (!main_translated) {
        translate_main_function(nullptr);
    }
}

void translator::translator::translate_main_function(std::unique_ptr<emitter::ir::func_decl_ir> main_decl_ir) {
    using namespace llvm;

    auto generate_empty_main = main_decl_ir == nullptr;

    auto main_func_type = FunctionType::get(Type::getInt32Ty(*context_), false);
    auto main_func = generate_empty_main ?
            Function::Create(
                    main_func_type,
                    GlobalValue::LinkageTypes::ExternalLinkage,
                    "main",
                    *module_) :
            translate_function(std::move(main_decl_ir));

    main_func->addFnAttr(Attribute::AttrKind::MustProgress);
    main_func->addFnAttr(Attribute::AttrKind::NoInline);
    main_func->addFnAttr(Attribute::AttrKind::NoRecurse);
    main_func->addFnAttr(Attribute::AttrKind::NoUnwind);
    main_func->addFnAttr(Attribute::AttrKind::OptimizeNone);
    main_func->addFnAttr("frame-pointer", "all");
    main_func->addFnAttr("min-legal-vector-width", "0");
    main_func->addFnAttr("no-trapping-math", "true");
    main_func->addFnAttr("stack-protector-buffer-size", "8");

    if (!generate_empty_main) {
        builder_->SetInsertPoint(current_allocation_block_, current_allocation_block_->begin());
        auto first_alloc = builder_->CreateAlloca(Type::getInt32Ty(*context_));
        builder_->ClearInsertionPoint();

        builder_->SetInsertPoint(current_allocation_block_, --current_allocation_block_->end());
        builder_->CreateStore(ConstantInt::get(Type::getInt32Ty(*context_), 0), first_alloc);
        builder_->ClearInsertionPoint();

        return;
    }

    auto exit_block = BasicBlock::Create(*context_, "exit", main_func);
    builder_->SetInsertPoint(exit_block);
    builder_->CreateRet(ConstantInt::get(Type::getInt32Ty(*context_), 0));
    builder_->ClearInsertionPoint();
}

llvm::Function* translator::translator::translate_function(std::unique_ptr<emitter::ir::func_decl_ir> func_decl_ir) {
    using namespace llvm;

    auto func = module_->getFunction(func_decl_ir->name);

    current_function_ = func;
    current_allocation_block_ = BasicBlock::Create(*context_, "allocation", func);

    auto entry_block = BasicBlock::Create(*context_, "entry", func);
    current_block_ = entry_block;

    translate_scope_stmt(func_decl_ir->scope.get());

    current_function_ = nullptr;

    builder_->SetInsertPoint(current_allocation_block_);
    builder_->CreateBr(entry_block);
    builder_->ClearInsertionPoint();

    func->addFnAttr("frame-pointer", "all");
    func->addFnAttr("stack-protector-buffer-size", "8");
    func->addFnAttr("no-trapping-math", "true");

    return func;
}

void translator::translator::translate_stmt(std::unique_ptr<emitter::ir::stmt_ir> stmt_ir) {
    if (dynamic_cast<emitter::ir::expr_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_expr_stmt(dynamic_cast<emitter::ir::expr_stmt_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::if_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_if_stmt(dynamic_cast<emitter::ir::if_stmt_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::while_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_while_stmt(dynamic_cast<emitter::ir::while_stmt_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::do_while_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_do_while_stmt(dynamic_cast<emitter::ir::do_while_stmt_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::loop_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_loop_stmt(dynamic_cast<emitter::ir::loop_stmt_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::scope_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_scope_stmt(dynamic_cast<emitter::ir::scope_stmt_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::let_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_let_stmt(dynamic_cast<emitter::ir::let_stmt_ir *>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::return_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_return_stmt(dynamic_cast<emitter::ir::return_stmt_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::break_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_break_stmt(dynamic_cast<emitter::ir::break_stmt_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::breakall_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_breakall_stmt();
        return;
    }

    if (dynamic_cast<emitter::ir::continue_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_continue_stmt();
        return;
    }

    utils::log_error("Unsupported statement type encountered, this should never happen!");
}

void translator::translator::translate_scope_stmt(emitter::ir::scope_stmt_ir *scope_stmt) {
    if (scope_stmt->parent_scope == nullptr) {
        insert_before_block_ = nullptr;
        break_to_blocks_.clear();
        breakall_to_block_ = nullptr;
        continue_to_block_ = nullptr;
    }

    auto scope_block = llvm::BasicBlock::Create(
            *context_, "scope", current_function_, insert_before_block_);
    auto after_scope_block = llvm::BasicBlock::Create(
            *context_, "after_scope", current_function_, insert_before_block_);
    insert_before_block_ = after_scope_block;

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(scope_block);
    builder_->ClearInsertionPoint();
    current_block_ = scope_block;

    current_scope_ = scope_stmt;
    auto new_translation_scope = new scope(translation_scope_);
    translation_scope_ = new_translation_scope;
    for (auto &stmt : scope_stmt->inner_stmts) {
        auto priv_insert_before_block = insert_before_block_;
        translate_stmt(std::move(stmt));
        insert_before_block_ = priv_insert_before_block;
    }
    current_scope_ = nullptr;
    translation_scope_ = translation_scope_->parent_scope;
    delete new_translation_scope;

    if (!br_generated_) {
        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(after_scope_block);
    } else {
        br_generated_ = false;
    }

    current_block_ = after_scope_block;

    if (scope_stmt->parent_scope == nullptr) {
        builder_->SetInsertPoint(after_scope_block);
        builder_->CreateUnreachable();
    }

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_expr_stmt(emitter::ir::expr_stmt_ir *expr_stmt) {
    builder_->SetInsertPoint(current_block_);
    translate_expr(expr_stmt->expr.get());
    builder_->ClearInsertionPoint();
}

void translator::translator::translate_if_stmt(emitter::ir::if_stmt_ir *if_stmt) {
    using namespace llvm;

    auto current_cond_block = BasicBlock::Create(
            *context_, "if_cond", current_function_, insert_before_block_);
    auto current_if_block = BasicBlock::Create(
            *context_, "if_body", current_function_, insert_before_block_);
    auto current_else_block = BasicBlock::Create(
            *context_, "else_body", current_function_, insert_before_block_);
    auto after_if_block = BasicBlock::Create(
            *context_, "after_if", current_function_, insert_before_block_);

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(current_cond_block);

    current_block_ = current_cond_block;
    insert_before_block_ = current_if_block;

    builder_->SetInsertPoint(current_block_);
    auto condition = translate_expr(if_stmt->if_expr.get());
    builder_->CreateCondBr(condition, current_if_block, current_else_block);

    current_block_ = current_if_block;
    insert_before_block_ = current_else_block;
    translate_scope_stmt(if_stmt->scope.get());

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(after_if_block);

    insert_before_block_ = after_if_block;

    for (auto &else_if_stmt: if_stmt->else_if_branches) {
        current_block_ = current_else_block;

        current_cond_block = BasicBlock::Create(
                *context_, "if_cond", current_function_, insert_before_block_);
        current_if_block = BasicBlock::Create(
                *context_, "if_body", current_function_, insert_before_block_);
        current_else_block = BasicBlock::Create(
                *context_, "else_body", current_function_, insert_before_block_);

        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(current_cond_block);

        current_block_ = current_cond_block;
        insert_before_block_ = current_if_block;

        builder_->SetInsertPoint(current_block_);
        condition = translate_expr(else_if_stmt->if_expr.get());
        builder_->CreateCondBr(condition, current_if_block, current_else_block);

        current_block_ = current_if_block;
        insert_before_block_ = current_else_block;
        translate_scope_stmt(else_if_stmt->scope.get());

        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(after_if_block);
    }

    if (if_stmt->else_branch) {
        current_block_ = current_else_block;
        insert_before_block_ = after_if_block;
        translate_scope_stmt(if_stmt->else_branch->scope.get());
    } else {
        current_block_ = current_else_block;
    }

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(after_if_block);

    current_block_ = after_if_block;

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_while_stmt(emitter::ir::while_stmt_ir *while_stmt) {
    using namespace llvm;

    inner_loops_++;

    auto condition_block = BasicBlock::Create(
            *context_, "while_cond", current_function_, insert_before_block_);
    auto while_block = BasicBlock::Create(
            *context_, "while_body", current_function_, insert_before_block_);
    auto after_while_block = BasicBlock::Create(
            *context_, "after_while", current_function_, insert_before_block_);

    if (inner_loops_ == 0) {
        breakall_to_block_ = after_while_block;
    }

    break_to_blocks_.push_back(after_while_block);
    auto priv_continue_to_block = continue_to_block_;
    continue_to_block_ = condition_block;

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(condition_block);

    current_block_ = condition_block;
    insert_before_block_ = while_block;
    builder_->SetInsertPoint(current_block_);
    auto condition = translate_expr(while_stmt->condition.get());
    builder_->CreateCondBr(condition, while_block, after_while_block);

    current_block_ = while_block;
    insert_before_block_ = after_while_block;
    translate_scope_stmt(while_stmt->scope.get());

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(condition_block);

    current_block_ = after_while_block;
    break_to_blocks_.erase(break_to_blocks_.begin() + inner_loops_);
    continue_to_block_ = priv_continue_to_block;

    if (inner_loops_ == 0) {
        breakall_to_block_ = nullptr;
    }

    inner_loops_--;

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_do_while_stmt(emitter::ir::do_while_stmt_ir *do_while_stmt) {
    using namespace llvm;

    inner_loops_++;

    auto do_while_block = BasicBlock::Create(
            *context_, "do_while_body", current_function_, insert_before_block_);
    auto condition_block = BasicBlock::Create(
            *context_, "do_while_cond", current_function_, insert_before_block_);
    auto after_do_while_block = BasicBlock::Create(
            *context_, "after_do_while", current_function_, insert_before_block_);

    if (inner_loops_ == 0) {
        breakall_to_block_ = after_do_while_block;
    }

    break_to_blocks_.push_back(after_do_while_block);
    auto priv_continue_to_block = continue_to_block_;
    continue_to_block_ = condition_block;

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(do_while_block);

    current_block_ = do_while_block;
    insert_before_block_ = condition_block;
    translate_scope_stmt(do_while_stmt->scope.get());

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(condition_block);

    current_block_ = condition_block;
    insert_before_block_ = after_do_while_block;
    builder_->SetInsertPoint(current_block_);
    auto condition = translate_expr(do_while_stmt->condition.get());
    builder_->CreateCondBr(condition, do_while_block, after_do_while_block);

    current_block_ = after_do_while_block;
    break_to_blocks_.erase(break_to_blocks_.begin() + inner_loops_);
    continue_to_block_ = priv_continue_to_block;

    if (inner_loops_ == 0) {
        breakall_to_block_ = nullptr;
    }

    inner_loops_--;

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_loop_stmt(emitter::ir::loop_stmt_ir *loop_stmt) {
    using namespace llvm;

    inner_loops_++;

    auto loop_block = BasicBlock::Create(
            *context_, "loop_body", current_function_, insert_before_block_);
    auto after_loop_block = BasicBlock::Create(
            *context_, "after_loop", current_function_, insert_before_block_);

    if (inner_loops_ == 0) {
        breakall_to_block_ = after_loop_block;
    }

    break_to_blocks_.push_back(after_loop_block);
    auto priv_continue_to_block = continue_to_block_;
    continue_to_block_ = loop_block;

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(loop_block);

    current_block_ = loop_block;
    insert_before_block_ = after_loop_block;
    translate_scope_stmt(loop_stmt->scope.get());

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(loop_block);

    current_block_ = after_loop_block;
    break_to_blocks_.erase(break_to_blocks_.begin() + inner_loops_);
    continue_to_block_ = priv_continue_to_block;

    if (inner_loops_ == 0) {
        breakall_to_block_ = nullptr;
    }

    inner_loops_--;

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_let_stmt(emitter::ir::let_stmt_ir* let_stmt) {
    builder_->SetInsertPoint(current_allocation_block_);
    auto allocated_var = builder_->CreateAlloca(
            types_[let_stmt->variable_type->name], nullptr, let_stmt->name);
    translation_scope_->local_variables[let_stmt->name] = allocated_var;
    builder_->ClearInsertionPoint();

    builder_->SetInsertPoint(current_block_);

    auto expr_result = translate_expr(let_stmt->expr.get());
    builder_->CreateStore(expr_result, allocated_var);

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_return_stmt(emitter::ir::return_stmt_ir* return_stmt) {
    builder_->SetInsertPoint(current_block_);

    auto expr_result = translate_expr(return_stmt->expr.get());
    builder_->CreateRet(expr_result);

    br_generated_ = true;

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_break_stmt(emitter::ir::break_stmt_ir* break_stmt) {
    builder_->SetInsertPoint(current_block_);
    br_generated_ = true;

    if (!break_stmt->break_expr) {
        builder_->CreateBr(break_to_blocks_.back());
    } else {
        using namespace llvm;

        auto break_expr_block = BasicBlock::Create(
                *context_, "break_expr", current_function_, insert_before_block_);
        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(break_expr_block);
        builder_->ClearInsertionPoint();

        std::vector<BasicBlock*> break_conds;
        for (int i = 0; i < break_to_blocks_.size() - 1; i++) {
            auto break_cond = BasicBlock::Create(
                    *context_, "break_cond", current_function_, insert_before_block_);
            break_conds.push_back(break_cond);

            builder_->SetInsertPoint(break_cond);
            auto current_break_to_block = break_to_blocks_[break_to_blocks_.size() - (i + 1)];
            builder_->CreateBr(current_break_to_block);
            builder_->ClearInsertionPoint();
        }

        current_block_ = break_expr_block;
        insert_before_block_ = break_conds[0];
        builder_->SetInsertPoint(current_block_);

        auto break_expr = translate_expr(break_stmt->break_expr.get());
        auto switch_inst = builder_->CreateSwitch(break_expr, breakall_to_block_, break_conds.size());
        for (int i = 0; i < break_conds.size(); i++) {
            auto break_cond = break_conds[i];
            switch_inst->addCase(
                    ConstantInt::get(Type::getInt32Ty(*context_), i + 1),
                    break_cond);
        }

        builder_->ClearInsertionPoint();
    }

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_breakall_stmt() {
    builder_->SetInsertPoint(current_block_);
    br_generated_ = true;
    builder_->CreateBr(breakall_to_block_);
    builder_->ClearInsertionPoint();
}

void translator::translator::translate_continue_stmt() {
    builder_->SetInsertPoint(current_block_);
    br_generated_ = true;
    builder_->CreateBr(continue_to_block_);
    builder_->ClearInsertionPoint();
}

llvm::Value *translator::translator::translate_expr(emitter::ir::expr_ir* expr) {
    using namespace emitter::ir;

    if (dynamic_cast<integer_expr_ir*>(expr) != nullptr) {
        return translate_int_expr(dynamic_cast<integer_expr_ir*>(expr));
    }

    if (dynamic_cast<boolean_expr_ir*>(expr) != nullptr) {
        return translate_boolean_expr(dynamic_cast<boolean_expr_ir*>(expr));
    }

    if (dynamic_cast<assignment_expr_ir*>(expr) != nullptr) {
        return translate_assignment_expr(dynamic_cast<assignment_expr_ir*>(expr));
    }

    if (dynamic_cast<binary_expr_ir*>(expr) != nullptr) {
        return translate_binary_expr(dynamic_cast<binary_expr_ir*>(expr));
    }

    if (dynamic_cast<unary_expr_ir*>(expr) != nullptr) {
        return translate_unary_expr(dynamic_cast<unary_expr_ir*>(expr));
    }

    if (dynamic_cast<call_expr_ir*>(expr) != nullptr) {
        return translate_call_expr(dynamic_cast<call_expr_ir*>(expr));
    }

    if (dynamic_cast<identifier_expr_ir*>(expr) != nullptr) {
        return translate_identifier_expr(dynamic_cast<identifier_expr_ir*>(expr));
    }

    if (dynamic_cast<cast_expr_ir*>(expr) != nullptr) {
        return translate_cast_expr(dynamic_cast<cast_expr_ir*>(expr));
    }

    utils::log_error("Unsupported expression found, exiting.");
    __builtin_unreachable();
}

llvm::Value *translator::translator::translate_assignment_expr(emitter::ir::assignment_expr_ir *assignment_expr) {
    auto expr_result = translate_expr(assignment_expr->inner_expr.get());

    if (assignment_expr->is_global) {
        auto global_var = module_->getNamedGlobal(assignment_expr->identifier_name);
        builder_->CreateStore(expr_result, global_var);
    } else {
        builder_->CreateStore(expr_result, translation_scope_->get_variable(assignment_expr->identifier_name));
    }

    return expr_result;
}

llvm::Constant *translator::translator::translate_int_expr(emitter::ir::integer_expr_ir* integer_expr) {
    return llvm::ConstantInt::get(types_[integer_expr->expr_type->name], integer_expr->number);
}

llvm::Constant *translator::translator::translate_boolean_expr(emitter::ir::boolean_expr_ir* boolean_expr) {
    return llvm::ConstantInt::get(types_[boolean_expr->expr_type->name], (int)boolean_expr->value);
}

llvm::Value *translator::translator::translate_unary_expr(emitter::ir::unary_expr_ir *unary_expr) {
    if (dynamic_cast<emitter::ir::prefix_expr_ir*>(unary_expr) != nullptr) {
        return translate_prefix_expr(dynamic_cast<emitter::ir::prefix_expr_ir*>(unary_expr));
    }

    if (dynamic_cast<emitter::ir::postfix_expr_ir*>(unary_expr) != nullptr) {
        return translate_postfix_expr(dynamic_cast<emitter::ir::postfix_expr_ir*>(unary_expr));
    }

    utils::log_error("Unsupported unary expression found, exiting with error.");
    __builtin_unreachable();
}

llvm::Value *translator::translator::translate_prefix_expr(emitter::ir::prefix_expr_ir *prefix_expr) {
    auto value = translate_expr(prefix_expr->expr.get());

    switch (prefix_expr->operation) {
        case emitter::ir::unary_operation_type::positive:
            return value;
        case emitter::ir::unary_operation_type::negative:
            return builder_->CreateNeg(value, "negated");
        case emitter::ir::unary_operation_type::bitwise_not:
            return builder_->CreateXor(
                    value,
                    llvm::ConstantInt::get(types_[prefix_expr->expr_type->name], -1, true),
                    "bitwise_not");
        case emitter::ir::unary_operation_type::logical_not:
            return builder_->CreateXor(
                    value, llvm::ConstantInt::get(types_["bool"], true), "not");
        case emitter::ir::unary_operation_type::increment:
            value = builder_->CreateAdd(
                    value,
                    llvm::ConstantInt::get(types_[prefix_expr->expr_type->name], 1),
                    "incremented");
            if (variable_encountered_) {
                variable_encountered_ = false;
                builder_->CreateStore(value, current_variable_);
            }
            return value;
        case emitter::ir::unary_operation_type::decrement:
            value = builder_->CreateSub(
                    value,
                    llvm::ConstantInt::get(types_[prefix_expr->expr_type->name], 1),
                    "decremented");
            if (variable_encountered_) {
                variable_encountered_ = false;
                builder_->CreateStore(value, current_variable_);
            }
            return value;
        default:
            utils::log_error("Unsupported prefix operator found, exiting with error.");
            __builtin_unreachable();
    }
}

llvm::Value *translator::translator::translate_postfix_expr(emitter::ir::postfix_expr_ir *postfix_expr) {
    auto value = translate_expr(postfix_expr->expr.get());

    llvm::Value* post_value;
    switch (postfix_expr->operation) {
        case emitter::ir::unary_operation_type::increment:
            post_value = builder_->CreateAdd(
                    value,
                    llvm::ConstantInt::get(types_[postfix_expr->expr_type->name], 1),
                    "incremented");
            if (variable_encountered_) {
                variable_encountered_ = false;
                builder_->CreateStore(post_value, current_variable_);
            }
            return value;
        case emitter::ir::unary_operation_type::decrement:
            post_value = builder_->CreateSub(
                    value,
                    llvm::ConstantInt::get(types_[postfix_expr->expr_type->name], 1),
                    "decremented");
            if (variable_encountered_) {
                variable_encountered_ = false;
                builder_->CreateStore(post_value, current_variable_);
            }
            return value;
        default:
            utils::log_error("Unsupported postfix operator found, exiting with error.");
            __builtin_unreachable();
    }
}

llvm::Value *translator::translator::translate_binary_expr(emitter::ir::binary_expr_ir* binary_expr) {
    using namespace emitter::ir;

    if (dynamic_cast<arithmetic_expr_ir*>(binary_expr)) {
        return translate_arithmetic_expr(dynamic_cast<arithmetic_expr_ir*>(binary_expr));
    }

    if (dynamic_cast<relational_expr_ir*>(binary_expr)) {
        return translate_relational_expr(dynamic_cast<relational_expr_ir*>(binary_expr));
    }

    if (dynamic_cast<logical_expr_ir*>(binary_expr)) {
        return translate_logical_expr(dynamic_cast<logical_expr_ir*>(binary_expr));
    }

    if (dynamic_cast<bitwise_expr_ir*>(binary_expr)) {
        return translate_bitwise_expr(dynamic_cast<bitwise_expr_ir*>(binary_expr));
    }

    utils::log_error("Unsupported binary expression found, this should never happen!");
    __builtin_unreachable();
}

llvm::Value *translator::translator::translate_arithmetic_expr(emitter::ir::arithmetic_expr_ir *arithmetic_expr) {
    using namespace llvm;

    auto left_expr = arithmetic_expr->left.get();
    auto right_expr = arithmetic_expr->right.get();

    auto left = translate_expr(left_expr);
    auto right = translate_expr(right_expr);

    if (dynamic_cast<emitter::ir::integer_type*>(left_expr->expr_type) == nullptr) {
        utils::log_error("Any arithmetical operation to non integer is now not supported.");
        __builtin_unreachable();
    }

    auto integer_type = dynamic_cast<emitter::ir::integer_type*>(left_expr->expr_type);

    switch (arithmetic_expr->operation_type) {
        case emitter::ir::addition:
            return builder_->CreateAdd(left, right);
        case emitter::ir::subtraction:
            return builder_->CreateSub(left, right);
        case emitter::ir::multiplication:
            return builder_->CreateMul(left, right);
        case emitter::ir::division:
            return integer_type->is_unsigned ?
                   builder_->CreateUDiv(left, right) :
                   builder_->CreateSDiv(left, right);
        case emitter::ir::binary_operation_type::modulus:
            return integer_type->is_unsigned ?
                   builder_->CreateURem(left, right) :
                   builder_->CreateSRem(left, right);
        default:
            utils::log_error("Malformed arithmetic expression found, this should never happen!");
            __builtin_unreachable();
    }
}

llvm::Value *translator::translator::translate_relational_expr(emitter::ir::relational_expr_ir *relational_expr) {
    using namespace llvm;

    auto left_expr = relational_expr->left.get();
    auto right_expr = relational_expr->right.get();

    auto left = translate_expr(left_expr);
    auto right = translate_expr(right_expr);

    if (dynamic_cast<emitter::ir::integer_type*>(left_expr->expr_type) == nullptr) {
        switch (relational_expr->operation_type) {
            case emitter::ir::binary_operation_type::equals_to:
                return builder_->CreateICmpEQ(left, right);
            case emitter::ir::binary_operation_type::not_equals_to:
                return builder_->CreateICmpNE(left, right);
            default:
                utils::log_error(
                        "Only equls and not equls relation operators are supported for any non integer type now.");
                __builtin_unreachable();
        }
    }

    auto integer_type = dynamic_cast<emitter::ir::integer_type*>(left_expr->expr_type);

    switch (relational_expr->operation_type) {
        case emitter::ir::binary_operation_type::equals_to:
            return builder_->CreateICmpEQ(left, right);
        case emitter::ir::binary_operation_type::not_equals_to:
            return builder_->CreateICmpNE(left, right);
        case emitter::ir::binary_operation_type::greater_than:
            return integer_type->is_unsigned ?
                   builder_->CreateICmpUGT(left, right) :
                   builder_->CreateICmpSGT(left, right);
        case emitter::ir::binary_operation_type::less_than:
            return integer_type->is_unsigned ?
                   builder_->CreateICmpULT(left, right) :
                   builder_->CreateICmpSLT(left, right);
        case emitter::ir::binary_operation_type::greater_or_equal:
            return integer_type->is_unsigned ?
                   builder_->CreateICmpUGE(left, right) :
                   builder_->CreateICmpSGE(left, right);
        case emitter::ir::binary_operation_type::less_or_equal:
            return integer_type->is_unsigned ?
                   builder_->CreateICmpULE(left, right) :
                   builder_->CreateICmpSLE(left, right);
        default:
            utils::log_error("Malformed relational expression found, this should never happen!");
            __builtin_unreachable();
    }
}

llvm::Value *translator::translator::translate_logical_expr(emitter::ir::logical_expr_ir *logical_expr) {
    using namespace llvm;

    if (logical_expr->operation_type == emitter::ir::binary_operation_type::logical_and) {
        auto logical_left_block = BasicBlock::Create(
                *context_, "logical_left", current_function_, insert_before_block_);
        auto logical_right_block = BasicBlock::Create(
                *context_, "logical_right", current_function_, insert_before_block_);
        auto logical_result_block = BasicBlock::Create(
                *context_, "logical_result", current_function_, insert_before_block_);

        auto left_expr = logical_expr->left.get();
        auto right_expr = logical_expr->right.get();

        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(logical_left_block);

        current_block_ = logical_left_block;
        priv_block_ = current_block_;
        insert_before_block_ = logical_right_block;

        builder_->SetInsertPoint(current_block_);

        auto left = translate_expr(left_expr);
        builder_->CreateCondBr(left, logical_right_block, logical_result_block);

        current_block_ = logical_right_block;
        builder_->SetInsertPoint(current_block_);
        auto right = translate_expr(right_expr);
        builder_->CreateBr(logical_result_block);

        current_block_ = logical_result_block;
        builder_->SetInsertPoint(current_block_);
        auto phi = builder_->CreatePHI(types_["bool"], 2);
        phi->addIncoming(ConstantInt::get(types_["bool"], false), priv_block_);
        phi->addIncoming(right, logical_right_block);

        priv_block_ = logical_result_block;

        return phi;
    }

    if (logical_expr->operation_type == emitter::ir::binary_operation_type::logical_or) {
        auto logical_left_block = BasicBlock::Create(
                *context_, "logical_left", current_function_, insert_before_block_);
        auto logical_right_block = BasicBlock::Create(
                *context_, "logical_right", current_function_, insert_before_block_);
        auto logical_result_block = BasicBlock::Create(
                *context_, "logical_result", current_function_, insert_before_block_);

        auto left_expr = logical_expr->left.get();
        auto right_expr = logical_expr->right.get();

        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(logical_left_block);

        current_block_ = logical_left_block;
        priv_block_ = current_block_;
        insert_before_block_ = logical_right_block;

        builder_->SetInsertPoint(current_block_);

        auto left = translate_expr(left_expr);
        builder_->CreateCondBr(left, logical_result_block, logical_right_block);

        current_block_ = logical_right_block;
        builder_->SetInsertPoint(current_block_);
        auto right = translate_expr(right_expr);
        builder_->CreateBr(logical_result_block);

        current_block_ = logical_result_block;
        builder_->SetInsertPoint(current_block_);
        auto phi = builder_->CreatePHI(types_["bool"], 2);
        phi->addIncoming(ConstantInt::get(types_["bool"], true), priv_block_);
        phi->addIncoming(right, logical_right_block);

        priv_block_ = logical_result_block;

        return phi;
    }

    utils::log_error("Malformed logical expression found, this should never happen!");
    __builtin_unreachable();
}

llvm::Value *translator::translator::translate_bitwise_expr(emitter::ir::bitwise_expr_ir *bitwise_expr) {
    using namespace llvm;

    auto left = translate_expr(bitwise_expr->left.get());
    auto right = translate_expr(bitwise_expr->right.get());

    switch (bitwise_expr->operation_type) {
        case emitter::ir::binary_operation_type::bitwise_and:
            return builder_->CreateAnd(left, right);
        case emitter::ir::binary_operation_type::bitwise_or:
            return builder_->CreateOr(left, right);
        case emitter::ir::binary_operation_type::bitwise_xor:
            return builder_->CreateXor(left, right);
        default:
            utils::log_error("Malformed bitwise expression found, this should never happen!");
            __builtin_unreachable();
    }
}

llvm::Value *translator::translator::translate_call_expr(emitter::ir::call_expr_ir *call_expr) {
    using namespace llvm;

    auto func = module_->getFunction(call_expr->function_name);

    std::vector<Value*> arguments_exprs;
    for (auto& argument : call_expr->arguments) {
        arguments_exprs.push_back(translate_expr(argument.get()));
    }

    return builder_->CreateCall(
            FunctionCallee(func->getFunctionType(), func),
            ArrayRef<Value*>(arguments_exprs));
}

llvm::Value *translator::translator::translate_identifier_expr(emitter::ir::identifier_expr_ir *identifier_expr) {
    if (dynamic_cast<emitter::ir::argument_exp_ir*>(identifier_expr) != nullptr) {
        return translate_argument_expr(dynamic_cast<emitter::ir::argument_exp_ir*>(identifier_expr));
    }

    auto type = types_[identifier_expr->expr_type->name];

    if (identifier_expr->is_global) {
        auto global_var = module_->getNamedGlobal(llvm::StringRef(identifier_expr->name));
        current_variable_ = global_var;
        variable_encountered_ = true;
        return builder_->CreateLoad(type, global_var);
    }

    current_variable_ = translation_scope_->get_variable(identifier_expr->name);
    variable_encountered_ = true;
    return builder_->CreateLoad(type, current_variable_);
}

llvm::Value *translator::translator::translate_argument_expr(emitter::ir::argument_exp_ir *argument_expr) {
    return current_function_->getArg(argument_expr->position);
}

llvm::Value *translator::translator::translate_cast_expr(emitter::ir::cast_expr_ir *cast_expr) {
    if (dynamic_cast<emitter::ir::upcast_expr_ir*>(cast_expr) != nullptr) {
        return translate_upcast_expr(dynamic_cast<emitter::ir::upcast_expr_ir*>(cast_expr));
    }

    if (dynamic_cast<emitter::ir::downcast_expr_ir*>(cast_expr) != nullptr) {
        return translate_downcast_expr(dynamic_cast<emitter::ir::downcast_expr_ir*>(cast_expr));
    }

    utils::log_error("Unsupported cast.");
    __builtin_unreachable();
}

llvm::Value *translator::translator::translate_upcast_expr(emitter::ir::upcast_expr_ir *upcast_expr) {
    if (upcast_expr->expr_type->is_basic && upcast_expr->inner_expr->expr_type->is_basic) {
        auto dest_type = types_[upcast_expr->expr_type->name];
        auto expr = translate_expr(upcast_expr->inner_expr.get());

        auto integer_type = dynamic_cast<emitter::ir::integer_type *>(upcast_expr->expr_type);

        if (integer_type->is_unsigned) {
            return builder_->CreateZExt(expr, dest_type, "upcasted");
        } else {
            return builder_->CreateSExt(expr, dest_type, "upcasted");
        }
    }

    utils::log_error("Upcasting non-basic types is not supported for now.");
    __builtin_unreachable();
}

llvm::Value *translator::translator::translate_downcast_expr(emitter::ir::downcast_expr_ir *downcast_expr) {
    if (downcast_expr->expr_type->is_basic && downcast_expr->inner_expr->expr_type->is_basic) {
        auto dest_type = types_[downcast_expr->expr_type->name];
        auto expr = translate_expr(downcast_expr->inner_expr.get());

        return builder_->CreateTrunc(expr, dest_type, "downcasted");
    }

     utils::log_error("Downcasting non-basic types is not supported for now.");
    __builtin_unreachable();
}
