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
                    let_stmt->name, std::move(expr_ir), expr_ir->expr_type);
            global_variables.push_back(std::move(variable_ir));
        }
    }

    return global_variables;
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
                    right_expr_ir->expr_type);
        }

        if (right_expr_ir->expr_type.can_be_explicitly_casted_to(left_expr_ir->expr_type)) {
            return std::make_unique<ir::binary_expr_ir>(
                    std::move(left_expr_ir),
                    std::move(right_expr_ir),
                    (ir::binary_operation_type)binary_expr->operation,
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
