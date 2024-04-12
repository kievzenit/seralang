#include "emitter.h"

std::unique_ptr<emitter::ir::package_ir> emitter::emitter::emit() {
    return std::make_unique<ir::package_ir>(
            translation_ast_->package->package_name,
            find_all_global_variables());
}

std::vector<std::unique_ptr<emitter::ir::variable_ir>> emitter::emitter::find_all_global_variables() {
    std::vector<std::unique_ptr<ir::variable_ir>> global_variables;

    for (auto &top_stmt : translation_ast_->stmts) {
        if (dynamic_cast<parser::ast::let_stmt*>(top_stmt.get()) != nullptr) {
            auto let_stmt = dynamic_cast<parser::ast::let_stmt*>(top_stmt.get());

            auto expr_ir = emit_for_expr(std::move(let_stmt->expression));
            auto variable_ir = std::make_unique<ir::variable_ir>(
                    let_stmt->name, std::move(expr_ir), ir::type::int32());
            global_variables.push_back(std::move(variable_ir));
        }
    }

    return global_variables;
}

std::unique_ptr<emitter::ir::expr_ir> emitter::emitter::emit_for_expr(std::unique_ptr<parser::ast::expr> expr) {
    if (dynamic_cast<parser::ast::integer_expr*>(expr.get()) != nullptr) {
        return std::make_unique<ir::integer_expr_ir>(dynamic_cast<parser::ast::integer_expr*>(expr.get()));
    }

    if (dynamic_cast<parser::ast::boolean_expr*>(expr.get()) != nullptr) {
        return std::make_unique<ir::boolean_expr_ir>(dynamic_cast<parser::ast::boolean_expr*>(expr.get()));
    }

    if (dynamic_cast<parser::ast::binary_expr*>(expr.get()) != nullptr) {
        auto binary_expr = dynamic_cast<parser::ast::binary_expr*>(expr.get());

        auto left_expr_ir = emit_for_expr(std::move(binary_expr->left));
        auto right_expr_ir = emit_for_expr(std::move(binary_expr->right));

        if (left_expr_ir->expr_type != right_expr_ir->expr_type) {
            utils::log_error("Left side of expression and right side are different types, so operation cannot be completed!");
        }

        return std::make_unique<ir::binary_expr_ir>(
                std::move(left_expr_ir),
                std::move(right_expr_ir),
                (ir::binary_operation_type)binary_expr->operation,
                left_expr_ir->expr_type);
    }

    utils::log_error("Unexpected expression expr_type, this should never happen!");
    __builtin_unreachable();
}
