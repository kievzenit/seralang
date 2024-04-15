#include "translator.h"

void translator::translator::translate() {
    using namespace llvm;

    create_types();
    translate_global_vars();

    auto main_func_type = FunctionType::get(Type::getInt32Ty(*context_), false);
    auto main_func = Function::Create(
            main_func_type,
            GlobalValue::LinkageTypes::ExternalLinkage,
            "main",
            *module_);

    auto init_block = BasicBlock::Create(*context_, "init", main_func);
    builder_->SetInsertPoint(init_block);

    builder_->CreateCall(FunctionCallee(init_globals_func_type_, init_globals_func_));

    auto exit_block = BasicBlock::Create(*context_, "exit", main_func);
    builder_->CreateBr(exit_block);

    builder_->ClearInsertionPoint();

    builder_->SetInsertPoint(exit_block);
    builder_->CreateRet(ConstantInt::get(Type::getInt32Ty(*context_), 0));
    builder_->ClearInsertionPoint();

    module_->dump();
}

void translator::translator::create_types() {
    create_basic_types();
}

void translator::translator::create_basic_types() {
    using namespace llvm;
    using namespace emitter::ir;

    types_[type::int1().name] = Type::getInt1Ty(*context_);
    types_[type::int8().name] = Type::getInt8Ty(*context_);
    types_[type::int16().name] = Type::getInt16Ty(*context_);
    types_[type::int32().name] = Type::getInt32Ty(*context_);
    types_[type::int64().name] = Type::getInt64Ty(*context_);
}

void translator::translator::translate_global_vars() {
    using namespace llvm;

    init_globals_func_type_ = FunctionType::get(Type::getVoidTy(*context_), false);
    init_globals_func_ = Function::Create(
            init_globals_func_type_,
            GlobalValue::LinkageTypes::PrivateLinkage,
            "init_globals",
            *module_);

    auto generate_br = false;
    for (auto &global_var : package_ir_->global_variables) {
        translate_global_var(std::move(global_var), generate_br);
    }

    auto exit_block = BasicBlock::Create(
            *context_,
            "exit",
            init_globals_func_);

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

    auto var_type = types_[variable_ir->variable_type.name];

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

    global_variables_[variable_ir->name] = variable;

    if (variable_ir->expr->is_const_expr) {
        return;
    }

    auto init_block = BasicBlock::Create(
            *context_,
            variable_ir->name + "_init",
            init_globals_func_);

    if (generate_br) {
        builder_->CreateBr(init_block);
    }

    builder_->SetInsertPoint(init_block);
    generate_br = true;

    auto result = translate_expr(variable_ir->expr.get());
    builder_->CreateStore(result, variable);
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
    return llvm::ConstantInt::get(types_[integer_expr->expr_type.name], integer_expr->number);
}

llvm::Constant *translator::translator::translate_boolean_expr(emitter::ir::boolean_expr_ir* boolean_expr) {
    return llvm::ConstantInt::get(types_[boolean_expr->expr_type.name], (int)boolean_expr->value);
}

llvm::Value *translator::translator::translate_binary_expr(emitter::ir::binary_expr_ir* binary_expr) {
    using namespace llvm;

    auto left = translate_expr(binary_expr->left.get());
    auto right = translate_expr(binary_expr->right.get());

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
