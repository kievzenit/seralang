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
    types_[type::int1()->name] = Type::getInt1Ty(*context_);
    types_[type::int8()->name] = Type::getInt8Ty(*context_);
    types_[type::int16()->name] = Type::getInt16Ty(*context_);
    types_[type::int32()->name] = Type::getInt32Ty(*context_);
    types_[type::int64()->name] = Type::getInt64Ty(*context_);
    types_[type::uint8()->name] = Type::getInt8Ty(*context_);
    types_[type::uint16()->name] = Type::getInt16Ty(*context_);
    types_[type::uint32()->name] = Type::getInt32Ty(*context_);
    types_[type::uint64()->name] = Type::getInt64Ty(*context_);
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
                PointerType::get(Type::getInt64PtrTy(*context_), 0),
                PointerType::get(Type::getInt64PtrTy(*context_), 0)
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
                                        Type::getInt64PtrTy(*context_),
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
        std::unique_ptr<emitter::ir::variable_ir> variable_ir,
        bool &generate_br) {
    using namespace llvm;

    auto var_type = types_[variable_ir->variable_type->name];

    auto initializer = variable_ir->expr->is_const_expr ?
                       (Constant*)translate_expr(variable_ir->expr.get()) :
                       ConstantInt::get(var_type, 0);

    auto variable = new GlobalVariable(
            *module_,
            var_type,
            false,
            variable_ir->is_private ?
                GlobalValue::LinkageTypes::InternalLinkage : GlobalValue::LinkageTypes::ExternalLinkage,
            initializer,
            variable_ir->name);

    variable->setAlignment(Align(variable_ir->variable_type->size)); // TODO: redo this

    if (variable_ir->expr->is_const_expr) {
        return;
    }

    auto init_globals_func = module_->getFunction("init_globals");
    auto init_block = BasicBlock::Create(
            *context_,
            variable_ir->name + "_init",
            init_globals_func);

    if (generate_br) {
        builder_->CreateBr(init_block);
    }

    builder_->SetInsertPoint(init_block);

    generate_br = true;
    auto result = translate_expr(variable_ir->expr.get());
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

    translate_scope_stmt(func_decl_ir->root_scope_stmt.get());

    local_variables_.clear();
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

    if (dynamic_cast<emitter::ir::scope_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_scope_stmt(dynamic_cast<emitter::ir::scope_stmt_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::variable_ir*>(stmt_ir.get()) != nullptr) {
        translate_var_stmt(dynamic_cast<emitter::ir::variable_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::assignment_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_assignment_stmt(dynamic_cast<emitter::ir::assignment_stmt_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::call_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_call_stmt(dynamic_cast<emitter::ir::call_stmt_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::return_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_return_stmt(dynamic_cast<emitter::ir::return_stmt_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::break_stmt_ir*>(stmt_ir.get()) != nullptr) {
        translate_break_stmt();
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

void translator::translator::translate_scope_stmt(emitter::ir::scope_stmt_ir *scope_ir) {
    current_scope_ = scope_ir;
    for (auto &stmt : scope_ir->inner_stmts) {
        auto priv_next_block = next_block_;
        translate_stmt(std::move(stmt));
        next_block_ = priv_next_block;
    }
    current_scope_ = nullptr;
}

void translator::translator::translate_if_stmt(emitter::ir::if_stmt_ir *if_stmt) {
    using namespace llvm;

    auto current_cond_block = BasicBlock::Create(
            *context_, "if_cond", current_function_, next_block_);
    auto current_if_block = BasicBlock::Create(
            *context_, "if_body", current_function_, next_block_);
    auto current_else_block = BasicBlock::Create(
            *context_, "else_body", current_function_, next_block_);
    auto after_if_block = BasicBlock::Create(
            *context_, "after_if", current_function_, next_block_);

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(current_cond_block);
    builder_->ClearInsertionPoint();

    current_block_ = current_cond_block;
    next_block_ = current_if_block;

    builder_->SetInsertPoint(current_block_);
    auto condition = translate_expr(if_stmt->if_expr.get());
    builder_->CreateCondBr(condition, current_if_block, current_else_block);
    builder_->ClearInsertionPoint();

    current_block_ = current_if_block;
    next_block_ = current_else_block;
    translate_scope_stmt(if_stmt->scope.get());

    if (!generating_br_from_loop_) {
        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(after_if_block);
        builder_->ClearInsertionPoint();
    } else {
        generating_br_from_loop_ = false;
    }

    next_block_ = after_if_block;

    for (auto& else_if_stmt : if_stmt->else_if_branches) {
        current_block_ = current_else_block;

        current_cond_block = BasicBlock::Create(
                *context_, "if_cond", current_function_, next_block_);
        current_if_block = BasicBlock::Create(
                *context_, "if_body", current_function_, next_block_);
        current_else_block = BasicBlock::Create(
                *context_, "else_body", current_function_, next_block_);

        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(current_cond_block);
        builder_->ClearInsertionPoint();

        current_block_ = current_cond_block;
        next_block_ = current_if_block;

        builder_->SetInsertPoint(current_block_);
        condition = translate_expr(else_if_stmt->if_expr.get());
        builder_->CreateCondBr(condition, current_if_block, current_else_block);
        builder_->ClearInsertionPoint();

        current_block_ = current_if_block;
        next_block_ = after_if_block;
        translate_scope_stmt(else_if_stmt->scope.get());

        if (!generating_br_from_loop_) {
            current_block_ = current_if_block;
            builder_->SetInsertPoint(current_block_);
            builder_->CreateBr(after_if_block);
            builder_->ClearInsertionPoint();
        } else {
            generating_br_from_loop_ = false;
        }
    }

    if (if_stmt->else_branch) {
        current_block_ = current_else_block;
        next_block_ = after_if_block;
        translate_scope_stmt(if_stmt->else_branch->scope.get());
    }

    if (!generating_br_from_loop_) {
        current_block_ = current_else_block;
        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(after_if_block);
        builder_->ClearInsertionPoint();
    } else {
        generating_br_from_loop_ = false;
    }

    current_block_ = after_if_block;
}

void translator::translator::translate_while_stmt(emitter::ir::while_stmt_ir *while_stmt) {
    using namespace llvm;

    inner_loops_++;

    auto condition_block = BasicBlock::Create(
            *context_, "while_cond", current_function_, next_block_);
    auto while_block = BasicBlock::Create(
            *context_, "while_body", current_function_, next_block_);
    auto after_while_block = BasicBlock::Create(
            *context_, "after_while", current_function_, next_block_);

    if (inner_loops_ == 0) {
        breakall_to_block_ = after_while_block;
    }

    auto priv_break_to_block = break_to_block_;
    break_to_block_ = after_while_block;
    auto priv_continue_to_block = continue_to_block_;
    continue_to_block_ = condition_block;

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(condition_block);

    current_block_ = condition_block;
    next_block_ = while_block;
    builder_->SetInsertPoint(current_block_);
    auto condition = translate_expr(while_stmt->condition.get());
    builder_->CreateCondBr(condition, while_block, after_while_block);

    current_block_ = while_block;
    next_block_ = after_while_block;
    translate_scope_stmt(while_stmt->scope.get());

    if (!generating_br_from_loop_) {
        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(condition_block);
    } else {
        generating_br_from_loop_ = false;
    }

    current_block_ = after_while_block;
    break_to_block_ = priv_break_to_block;
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
            *context_, "do_while_body", current_function_, next_block_);
    auto condition_block = BasicBlock::Create(
            *context_, "do_while_cond", current_function_, next_block_);
    auto after_do_while_block = BasicBlock::Create(
            *context_, "after_do_while", current_function_, next_block_);

    if (inner_loops_ == 0) {
        breakall_to_block_ = after_do_while_block;
    }

    auto priv_break_to_block = break_to_block_;
    break_to_block_ = after_do_while_block;
    auto priv_continue_to_block = continue_to_block_;
    continue_to_block_ = condition_block;

    builder_->SetInsertPoint(current_block_);
    builder_->CreateBr(do_while_block);

    current_block_ = do_while_block;
    next_block_ = condition_block;
    translate_scope_stmt(do_while_stmt->scope.get());

    if (!generating_br_from_loop_) {
        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(condition_block);
    } else {
        generating_br_from_loop_ = false;
    }

    current_block_ = condition_block;
    next_block_ = after_do_while_block;
    builder_->SetInsertPoint(current_block_);
    auto condition = translate_expr(do_while_stmt->condition.get());
    builder_->CreateCondBr(condition, do_while_block, after_do_while_block);

    current_block_ = after_do_while_block;
    break_to_block_ = priv_break_to_block;
    continue_to_block_ = priv_continue_to_block;

    if (inner_loops_ == 0) {
        breakall_to_block_ = nullptr;
    }

    inner_loops_--;

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_var_stmt(emitter::ir::variable_ir* variable_ir) {
    builder_->SetInsertPoint(current_allocation_block_);
    auto allocated_var = builder_->CreateAlloca(
            types_[variable_ir->variable_type->name], nullptr, variable_ir->name);
    local_variables_[variable_ir->name] = allocated_var;
    builder_->ClearInsertionPoint();

    builder_->SetInsertPoint(current_block_);

    auto expr_result = translate_expr(variable_ir->expr.get());
    builder_->CreateStore(expr_result, allocated_var);

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_assignment_stmt(emitter::ir::assignment_stmt_ir *assignment_stmt) {
    using namespace llvm;

    builder_->SetInsertPoint(current_block_);

    auto expr_result = translate_expr(assignment_stmt->assignment_expr.get());

    if (assignment_stmt->is_global) {
        auto global_var = module_->getNamedGlobal(assignment_stmt->identifier_name);
        builder_->CreateStore(expr_result, global_var);
    } else {
        builder_->CreateStore(expr_result, local_variables_[assignment_stmt->identifier_name]);
    }

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_call_stmt(emitter::ir::call_stmt_ir *call_stmt) {
    using namespace llvm;

    builder_->SetInsertPoint(current_block_);
    translate_call_expr(call_stmt->call_expr.get());
    builder_->ClearInsertionPoint();
}

void translator::translator::translate_return_stmt(emitter::ir::return_stmt_ir* return_ir) {
    builder_->SetInsertPoint(current_block_);

    auto expr_result = translate_expr(return_ir->expr.get());
    builder_->CreateRet(expr_result);

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_break_stmt() {
    builder_->SetInsertPoint(current_block_);
    generating_br_from_loop_ = true;
    builder_->CreateBr(break_to_block_);
    builder_->ClearInsertionPoint();
}

void translator::translator::translate_breakall_stmt() {
    builder_->SetInsertPoint(current_block_);
    generating_br_from_loop_ = true;
    builder_->CreateBr(breakall_to_block_);
    builder_->ClearInsertionPoint();
}

void translator::translator::translate_continue_stmt() {
    builder_->SetInsertPoint(current_block_);
    generating_br_from_loop_ = true;
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

    if (dynamic_cast<binary_expr_ir*>(expr) != nullptr) {
        return translate_binary_expr(dynamic_cast<binary_expr_ir*>(expr));
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

llvm::Constant *translator::translator::translate_int_expr(emitter::ir::integer_expr_ir* integer_expr) {
    return llvm::ConstantInt::get(types_[integer_expr->expr_type->name], integer_expr->number);
}

llvm::Constant *translator::translator::translate_boolean_expr(emitter::ir::boolean_expr_ir* boolean_expr) {
    return llvm::ConstantInt::get(types_[boolean_expr->expr_type->name], (int)boolean_expr->value);
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

    switch (arithmetic_expr->operation_type) {
        case emitter::ir::addition:
            return builder_->CreateAdd(left, right);
        case emitter::ir::subtraction:
            return builder_->CreateSub(left, right);
        case emitter::ir::multiplication:
            return builder_->CreateMul(left, right);
        case emitter::ir::division:
            return left_expr->expr_type->is_unsigned ?
                   builder_->CreateUDiv(left, right) :
                   builder_->CreateSDiv(left, right);
        case emitter::ir::binary_operation_type::modulus:
            return left_expr->expr_type->is_unsigned ?
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

    switch (relational_expr->operation_type) {
        case emitter::ir::binary_operation_type::equals_to:
            return builder_->CreateICmpEQ(left, right);
        case emitter::ir::binary_operation_type::not_equals_to:
            return builder_->CreateICmpNE(left, right);
        case emitter::ir::binary_operation_type::greater_than:
            return left_expr->expr_type->is_unsigned ?
                   builder_->CreateICmpUGT(left, right) :
                   builder_->CreateICmpSGT(left, right);
        case emitter::ir::binary_operation_type::less_than:
            return left_expr->expr_type->is_unsigned ?
                   builder_->CreateICmpULT(left, right) :
                   builder_->CreateICmpSLT(left, right);
        case emitter::ir::binary_operation_type::greater_or_equal:
            return left_expr->expr_type->is_unsigned ?
                   builder_->CreateICmpUGE(left, right) :
                   builder_->CreateICmpSGE(left, right);
        case emitter::ir::binary_operation_type::less_or_equal:
            return left_expr->expr_type->is_unsigned ?
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
                *context_, "logical_left", current_function_, next_block_);
        auto logical_right_block = BasicBlock::Create(
                *context_, "logical_right", current_function_, next_block_);
        auto logical_result_block = BasicBlock::Create(
                *context_, "logical_result", current_function_, next_block_);

        auto left_expr = logical_expr->left.get();
        auto right_expr = logical_expr->right.get();

        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(logical_left_block);

        current_block_ = logical_left_block;
        priv_block_ = current_block_;
        next_block_ = logical_right_block;

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
        next_block_ = nullptr;

        return phi;
    }

    if (logical_expr->operation_type == emitter::ir::binary_operation_type::logical_or) {
        auto logical_left_block = BasicBlock::Create(
                *context_, "logical_left", current_function_, next_block_);
        auto logical_right_block = BasicBlock::Create(
                *context_, "logical_right", current_function_, next_block_);
        auto logical_result_block = BasicBlock::Create(
                *context_, "logical_result", current_function_, next_block_);

        auto left_expr = logical_expr->left.get();
        auto right_expr = logical_expr->right.get();

        builder_->SetInsertPoint(current_block_);
        builder_->CreateBr(logical_left_block);

        current_block_ = logical_left_block;
        priv_block_ = current_block_;
        next_block_ = logical_right_block;

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
        next_block_ = nullptr;

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
        return builder_->CreateLoad(type, global_var);
    }

    return builder_->CreateLoad(type, local_variables_[identifier_expr->name]);
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

        if (upcast_expr->expr_type->is_unsigned) {
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
