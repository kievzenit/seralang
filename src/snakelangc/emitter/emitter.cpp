#include "emitter.h"

std::unique_ptr<emitter::ir::package_ir> emitter::emitter::emit() {
    auto global_variables = find_all_global_variables();
    auto func_declarations = find_all_func_declarations();

    return std::make_unique<ir::package_ir>(
            translation_ast_->package->package_name,
            std::move(global_variables),
            std::move(func_declarations));
}

bool emitter::emitter::is_global_variable_exists(const std::string& name) {
    return std::find(global_variables_.begin(), global_variables_.end(), name) != global_variables_.end();
}

bool emitter::emitter::is_function_declared(const std::string& name) {
    return std::find(functions_.begin(), functions_.end(), name) != functions_.end();
}

std::vector<std::unique_ptr<emitter::ir::variable_ir>> emitter::emitter::find_all_global_variables() {
    std::vector<std::unique_ptr<ir::variable_ir>> global_variables;

    for (auto &top_stmt : translation_ast_->stmts) {
        if (dynamic_cast<parser::ast::let_stmt*>(top_stmt.get()) == nullptr) {
            continue;
        }

        auto let_stmt = dynamic_cast<parser::ast::let_stmt*>(top_stmt.get());

        if (is_global_variable_exists(let_stmt->name)) {
            utils::log_error(
                    std::format("Global variable with name: {} is already defined.", let_stmt->name));
        }

        auto expr_ir = emit_for_expr(std::move(let_stmt->expression));
        auto variable_ir = std::make_unique<ir::variable_ir>(
                let_stmt->name, std::move(expr_ir), expr_ir->expr_type);
        global_variables.push_back(std::move(variable_ir));
    }

    return global_variables;
}

std::vector<std::unique_ptr<emitter::ir::func_decl_ir>> emitter::emitter::find_all_func_declarations() {
    std::vector<std::unique_ptr<ir::func_decl_ir>> func_decls;

    for (auto &top_stmt : translation_ast_->stmts) {
        if (dynamic_cast<parser::ast::func_decl_stmt*>(top_stmt.get()) == nullptr) {
            continue;
        }

        auto func_decl_stmt = dynamic_cast<parser::ast::func_decl_stmt*>(top_stmt.get());

        if (is_function_declared(func_decl_stmt->name)) {
            utils::log_error(
                    std::format("Function with name: {} is already defined.", func_decl_stmt->name));
        }

        auto type = types_.find(func_decl_stmt->return_type);
        if (type == types_.end()) {
            utils::log_error(
                    std::format(
                            "Unknown type: {} specified for result type.",
                            func_decl_stmt->return_type));
        }

        auto root_scope_stmt_ir = emit_for_scope_stmt(func_decl_stmt->func_scope.get());

        auto last_stmt_ir = root_scope_stmt_ir->inner_stmts.back().get();
        auto return_stmt_ir = dynamic_cast<ir::return_ir*>(last_stmt_ir);
        if (return_stmt_ir == nullptr) {
            utils::log_error("Expected return in the end of function, but got nothing instead.");
            __builtin_unreachable();
        }

        auto func_decl_ir = std::make_unique<ir::func_decl_ir>(
                func_decl_stmt->name,
                type->second,
                std::move(root_scope_stmt_ir));

        if (return_stmt_ir->expr->expr_type == type->second) {
            func_decls.push_back(std::move(func_decl_ir));
            continue;
        }

        if (!return_stmt_ir->expr->expr_type.can_be_explicitly_casted_to(type->second)) {
            utils::log_error("Incompatible return expression type with function return type.");
        }

        func_decls.push_back(std::move(func_decl_ir));
    }

    return func_decls;
}

std::unique_ptr<emitter::ir::scope_stmt_ir> emitter::emitter::emit_for_scope_stmt(parser::ast::scope_stmt* scope_stmt) {
    std::vector<std::unique_ptr<ir::stmt_ir>> stmts;

    for (auto &stmt : scope_stmt->inner_stmts) {
        auto stmt_ir = emit_for_stmt(std::move(stmt));
        stmts.push_back(std::move(stmt_ir));
    }

    return std::make_unique<ir::scope_stmt_ir>(std::move(stmts));
}

std::unique_ptr<emitter::ir::stmt_ir> emitter::emitter::emit_for_stmt(std::unique_ptr<parser::ast::stmt> stmt) {
    if (dynamic_cast<parser::ast::scope_stmt*>(stmt.get()) != nullptr) {
        return emit_for_scope_stmt(dynamic_cast<parser::ast::scope_stmt*>(stmt.get()));
    }

    if (dynamic_cast<parser::ast::let_stmt*>(stmt.get()) != nullptr) {
        auto let_stmt = dynamic_cast<parser::ast::let_stmt*>(stmt.get());

        auto expr_ir = emit_for_expr(std::move(let_stmt->expression));
        auto var_ir = std::make_unique<ir::variable_ir>(
                let_stmt->name,
                std::move(expr_ir),
                expr_ir->expr_type);

        return std::move(var_ir);
    }

    if (dynamic_cast<parser::ast::return_stmt*>(stmt.get()) != nullptr) {
        auto return_stmt = dynamic_cast<parser::ast::return_stmt*>(stmt.get());
        return std::make_unique<ir::return_ir>(emit_for_expr(std::move(return_stmt->return_expr)));
    }

    utils::log_error("Unsupported statement encountered, this should never happen!");
    __builtin_unreachable();
}

std::unique_ptr<emitter::ir::expr_ir> emitter::emitter::emit_for_expr(std::unique_ptr<parser::ast::expr> expr) {
    if (dynamic_cast<parser::ast::integer_expr*>(expr.get()) != nullptr) {
        auto integer_expr = dynamic_cast<parser::ast::integer_expr*>(expr.get());
        auto explicit_int_type = integer_expr->explicit_int_type;
        if (explicit_int_type.empty()) {
            return std::make_unique<ir::integer_expr_ir>(integer_expr);
        }

        return emit_for_explicitly_typed_integer(integer_expr, explicit_int_type);
    }

    if (dynamic_cast<parser::ast::boolean_expr*>(expr.get()) != nullptr) {
        return std::make_unique<ir::boolean_expr_ir>(dynamic_cast<parser::ast::boolean_expr*>(expr.get()));
    }

    if (dynamic_cast<parser::ast::binary_expr*>(expr.get()) != nullptr) {
        auto binary_expr = dynamic_cast<parser::ast::binary_expr*>(expr.get());

        auto left_expr_ir = emit_for_expr(std::move(binary_expr->left));
        auto right_expr_ir = emit_for_expr(std::move(binary_expr->right));

        if (left_expr_ir->expr_type == right_expr_ir->expr_type) {
            return std::make_unique<ir::binary_expr_ir>(
                    std::move(left_expr_ir),
                    std::move(right_expr_ir),
                    (ir::binary_operation_type)binary_expr->operation,
                    left_expr_ir->expr_type);
        }

        if (left_expr_ir->expr_type.can_be_explicitly_casted_to(right_expr_ir->expr_type)) {
            return std::make_unique<ir::binary_expr_ir>(
                    std::move(left_expr_ir),
                    std::move(right_expr_ir),
                    (ir::binary_operation_type)binary_expr->operation,
                    true,
                    false,
                    right_expr_ir->expr_type);
        }

        if (right_expr_ir->expr_type.can_be_explicitly_casted_to(left_expr_ir->expr_type)) {
            return std::make_unique<ir::binary_expr_ir>(
                    std::move(left_expr_ir),
                    std::move(right_expr_ir),
                    (ir::binary_operation_type)binary_expr->operation,
                    false,
                    true,
                    left_expr_ir->expr_type);
        }

        utils::log_error("Left and right side of expression cannot be explicitly casted to other side.");
    }

    utils::log_error("Unexpected expression expr_type, this should never happen!");
    __builtin_unreachable();
}

std::unique_ptr<emitter::ir::integer_expr_ir> emitter::emitter::emit_for_explicitly_typed_integer(
        parser::ast::integer_expr* integer_expr,
        const std::string& explicit_int_type) {
    if (explicit_int_type == "int1") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::type::int1());
    }

    if (explicit_int_type == "int8") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::type::int8());
    }

    if (explicit_int_type == "int16") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::type::int16());
    }

    if (explicit_int_type == "int32") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::type::int32());
    }

    if (explicit_int_type == "int64") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::type::int64());
    }

    if (explicit_int_type == "uint8") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::type::uint8());
    }

    if (explicit_int_type == "uint16") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::type::uint16());
    }

    if (explicit_int_type == "uint32") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::type::uint32());
    }

    if (explicit_int_type == "uint64") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::type::uint64());
    }

    utils::log_error(std::format("Expected int type, but got: {} instead.", explicit_int_type));
    __builtin_unreachable();
}
