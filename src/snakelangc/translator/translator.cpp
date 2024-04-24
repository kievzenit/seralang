#include "translator.h"

void translator::translator::translate() {
    using namespace llvm;

    create_types();

    translate_global_vars();
    translate_function_declarations();

    module_->dump();
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
            GlobalValue::LinkageTypes::InternalLinkage,
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
        return;
    }

    auto exit_block = BasicBlock::Create(*context_, "exit", main_func);
    builder_->SetInsertPoint(exit_block);
    builder_->CreateRet(ConstantInt::get(Type::getInt32Ty(*context_), 0));
    builder_->ClearInsertionPoint();
}

llvm::Function* translator::translator::translate_function(std::unique_ptr<emitter::ir::func_decl_ir> func_decl_ir) {
    using namespace llvm;

    auto func_type = FunctionType::get(types_[func_decl_ir->return_type->name], false);
    auto func = Function::Create(
            func_type, GlobalValue::LinkageTypes::ExternalLinkage, func_decl_ir->name, *module_);

    current_allocation_block_ = BasicBlock::Create(*context_, "allocation", func);

    builder_->SetInsertPoint(current_allocation_block_);
    auto first_alloc = builder_->CreateAlloca(Type::getInt32Ty(*context_));
    builder_->ClearInsertionPoint();

    auto entry_block = BasicBlock::Create(*context_, "entry", func);
    current_block_ = entry_block;

    for (auto &stmt : func_decl_ir->root_scope_stmt->inner_stmts) {
        translate_stmt(std::move(stmt));
    }

    builder_->SetInsertPoint(current_allocation_block_);
    builder_->CreateStore(ConstantInt::get(Type::getInt32Ty(*context_), 0), first_alloc);
    builder_->CreateBr(entry_block);
    builder_->ClearInsertionPoint();

    func->addFnAttr("frame-pointer", "all");
    func->addFnAttr("stack-protector-buffer-size", "8");

    return func;
}

void translator::translator::translate_stmt(std::unique_ptr<emitter::ir::stmt_ir> stmt_ir) {
    if (dynamic_cast<emitter::ir::variable_ir*>(stmt_ir.get()) != nullptr) {
        translate_var_stmt(dynamic_cast<emitter::ir::variable_ir*>(stmt_ir.get()));
        return;
    }

    if (dynamic_cast<emitter::ir::return_ir*>(stmt_ir.get()) != nullptr) {
        translate_return_stmt(dynamic_cast<emitter::ir::return_ir*>(stmt_ir.get()));
        return;
    }

    utils::log_error("Unsupported statement type encountered, this should never happen!");
}

void translator::translator::translate_var_stmt(emitter::ir::variable_ir* variable_ir) {
    builder_->SetInsertPoint(current_allocation_block_);
    auto allocated_var = builder_->CreateAlloca(
            types_[variable_ir->variable_type->name], nullptr, variable_ir->name);
    builder_->ClearInsertionPoint();

    builder_->SetInsertPoint(current_block_);

    auto expr_result = translate_expr(variable_ir->expr.get());
    builder_->CreateStore(expr_result, allocated_var);

    builder_->ClearInsertionPoint();
}

void translator::translator::translate_return_stmt(emitter::ir::return_ir* return_ir) {
    builder_->SetInsertPoint(current_block_);

    auto expr_result = translate_expr(return_ir->expr.get());
    builder_->CreateRet(expr_result);

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
    using namespace llvm;

    auto left_expr = binary_expr->left.get();
    auto right_expr = binary_expr->right.get();

    auto left = translate_expr(left_expr);
    auto right = translate_expr(right_expr);

    if (binary_expr->left_should_be_upcasted) {
        left = builder_->CreateSExt(left, types_[right_expr->expr_type->name]);
    } else if (binary_expr->right_should_be_upcasted) {
        right = builder_->CreateSExt(right, types_[left_expr->expr_type->name]);
    }

    switch (binary_expr->operation_type) {
        case emitter::ir::addition:
            return builder_->CreateAdd(left, right);
        case emitter::ir::subtraction:
            return builder_->CreateSub(left, right);
        case emitter::ir::multiplication:
            return builder_->CreateMul(left, right);
        case emitter::ir::division:
            return builder_->CreateSDiv(left, right);
        default:
            utils::log_error("Unsupported binary operation encountered, exiting with error.");
            __builtin_unreachable();
    }
}
