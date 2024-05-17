#include "emitter.h"

std::unique_ptr<emitter::ir::expr_ir> emitter::emitter::emit_for_expr(std::unique_ptr<parser::ast::expr> expr) {
    if (dynamic_cast<parser::ast::integer_expr*>(expr.get()) != nullptr) {
        return emit_for_integer_expr(dynamic_cast<parser::ast::integer_expr*>(expr.get()));
    }

    if (dynamic_cast<parser::ast::boolean_expr*>(expr.get()) != nullptr) {
        return emit_for_boolean_expr(dynamic_cast<parser::ast::boolean_expr*>(expr.get()));
    }

    if (dynamic_cast<parser::ast::call_expr*>(expr.get()) != nullptr) {
        return emit_for_call_expr(dynamic_cast<parser::ast::call_expr*>(expr.get()));
    }

    if (dynamic_cast<parser::ast::identifier_expr*>(expr.get()) != nullptr) {
        return emit_for_identifier_expr(dynamic_cast<parser::ast::identifier_expr*>(expr.get()));
    }

    if (dynamic_cast<parser::ast::assignment_expr*>(expr.get()) != nullptr) {
        return emit_for_assignment_expr(dynamic_cast<parser::ast::assignment_expr*>(expr.get()));
    }

    if (dynamic_cast<parser::ast::binary_expr*>(expr.get()) != nullptr) {
        return emit_for_binary_expr(dynamic_cast<parser::ast::binary_expr*>(expr.get()));
    }

    if (dynamic_cast<parser::ast::unary_expr*>(expr.get()) != nullptr) {
        return emit_for_unary_expr(dynamic_cast<parser::ast::unary_expr*>(expr.get()));
    }

    utils::log_error("Unexpected expression expr_type, this should never happen!");
    __builtin_unreachable();
}

std::unique_ptr<emitter::ir::assignment_expr_ir>
emitter::emitter::emit_for_assignment_expr(parser::ast::assignment_expr *assignment_expr) {
    auto expr = emit_for_expr(std::move(assignment_expr->inner_expr));

    if (current_scope_->is_var_exists(assignment_expr->name)) {
        auto identifier_type = current_scope_->get_type_for_variable(assignment_expr->name);
        return std::make_unique<ir::assignment_expr_ir>(
                assignment_expr->name,
                false,
                emit_for_cast(std::move(expr), identifier_type));
    }

    if (declared_global_variables_.contains(assignment_expr->name)) {
        auto global_var_type = global_variables_types_[assignment_expr->name];
        return std::make_unique<ir::assignment_expr_ir>(
                assignment_expr->name,
                true,
                emit_for_cast(std::move(expr), global_var_type));
    }

    auto generated_name = generate_func_static_var_name(assignment_expr->name);
    if (declared_global_variables_.contains(generated_name)) {
        auto global_var_type = global_variables_types_[generated_name];
        return std::make_unique<ir::assignment_expr_ir>(
                generated_name,
                true,
                emit_for_cast(std::move(expr), global_var_type));
    }

    utils::log_error(std::format("Undefined identifier: {}.", assignment_expr->name));
    __builtin_unreachable();
}

std::unique_ptr<emitter::ir::unary_expr_ir>
emitter::emitter::emit_for_unary_expr(parser::ast::unary_expr *unary_expr) {
    if (dynamic_cast<parser::ast::prefix_expr*>(unary_expr) != nullptr) {
        return emit_for_prefix_expr(dynamic_cast<parser::ast::prefix_expr*>(unary_expr));
    }

    if (dynamic_cast<parser::ast::postfix_expr*>(unary_expr) != nullptr) {
        return emit_for_postfix_expr(dynamic_cast<parser::ast::postfix_expr*>(unary_expr));
    }

    utils::log_error("Unexpected unary expression type, not postfix and not prefix.");
    __builtin_unreachable();
}

std::unique_ptr<emitter::ir::prefix_expr_ir>
emitter::emitter::emit_for_prefix_expr(parser::ast::prefix_expr *prefix_expr) {
    auto inner_expr = emit_for_expr(std::move(prefix_expr->inner_expr));
    if (!inner_expr->expr_type->is_basic) {
        utils::log_error("Only basic types are supported with prefix expressions for now(.");
    }

    switch (prefix_expr->operation) {
        case parser::ast::unary_operation::logical_not:
            inner_expr = emit_for_cast(std::move(inner_expr), types_["bool"]);
        default:
            break;
    }

    return std::make_unique<ir::prefix_expr_ir>(
            std::move(inner_expr),
            (ir::unary_operation_type)prefix_expr->operation);
}

std::unique_ptr<emitter::ir::postfix_expr_ir>
emitter::emitter::emit_for_postfix_expr(parser::ast::postfix_expr *postfix_expr) {
    auto inner_expr = emit_for_expr(std::move(postfix_expr->inner_expr));
    if (!inner_expr->expr_type->is_basic) {
        utils::log_error("Only basic types are supported with postfix expressions for now(.");
    }

    return std::make_unique<ir::postfix_expr_ir>(
            std::move(inner_expr),
            (ir::unary_operation_type)postfix_expr->operation);
}

std::unique_ptr<emitter::ir::binary_expr_ir>
emitter::emitter::emit_for_binary_expr(parser::ast::binary_expr *binary_expr) {
    auto left_expr_ir = emit_for_expr(std::move(binary_expr->left));
    auto right_expr_ir = emit_for_expr(std::move(binary_expr->right));

    std::tuple<std::unique_ptr<ir::expr_ir>, std::unique_ptr<ir::expr_ir>, ir::type*> result;

    switch (binary_expr->operation) {
        case parser::ast::binary_operation::logical_and:
        case parser::ast::binary_operation::logical_or:
            left_expr_ir = emit_for_cast(std::move(left_expr_ir), types_["bool"]);
            right_expr_ir = emit_for_cast(std::move(right_expr_ir), types_["bool"]);
            return std::make_unique<ir::logical_expr_ir>(
                    std::move(left_expr_ir),
                    std::move(right_expr_ir),
                    (ir::binary_operation_type)binary_expr->operation);
        case parser::ast::binary_operation::equals_to:
        case parser::ast::binary_operation::not_equals_to:
        case parser::ast::binary_operation::greater_than:
        case parser::ast::binary_operation::less_than:
        case parser::ast::binary_operation::greater_or_equal:
        case parser::ast::binary_operation::less_or_equal:
            result = emit_for_cast(std::move(left_expr_ir), std::move(right_expr_ir));
            return std::make_unique<ir::relational_expr_ir>(
                    std::move(std::get<0>(result)),
                    std::move(std::get<1>(result)),
                    (ir::binary_operation_type)binary_expr->operation);
        case parser::ast::binary_operation::bitwise_and:
        case parser::ast::binary_operation::bitwise_or:
        case parser::ast::binary_operation::bitwise_xor:
            result = emit_for_cast(std::move(left_expr_ir), std::move(right_expr_ir));
            return std::make_unique<ir::bitwise_expr_ir>(
                    std::move(std::get<0>(result)),
                    std::move(std::get<1>(result)),
                    (ir::binary_operation_type)binary_expr->operation,
                    std::get<2>(result));
        case parser::ast::binary_operation::plus:
        case parser::ast::binary_operation::minus:
        case parser::ast::binary_operation::multiply:
        case parser::ast::binary_operation::divide:
        case parser::ast::binary_operation::modulus:
            result = emit_for_cast(std::move(left_expr_ir), std::move(right_expr_ir));
            return std::make_unique<ir::arithmetic_expr_ir>(
                    std::move(std::get<0>(result)),
                    std::move(std::get<1>(result)),
                    (ir::binary_operation_type)binary_expr->operation,
                    std::get<2>(result));
        default:
            utils::log_error("Unsupported binary expr encountered, this should never happen!");
            __builtin_unreachable();
    }
}

std::unique_ptr<emitter::ir::expr_ir>
emitter::emitter::emit_for_cast(std::unique_ptr<ir::expr_ir> expr, ir::type *cast_to) {
    if (expr->expr_type == cast_to) {
        return expr;
    }

    if (expr->expr_type->can_be_implicitly_casted_to(cast_to)) {
        if (!expr->expr_type->is_basic) {
            utils::log_error("Casting non-basic type is not supported for now.");
        }

        return emit_for_upcast(std::move(expr), cast_to);
    }

    utils::log_error(std::format("Expression cannot be explicitly casted to type: {}.", cast_to->name));
    __builtin_unreachable();
}

std::tuple<std::unique_ptr<emitter::ir::expr_ir>, std::unique_ptr<emitter::ir::expr_ir>, emitter::ir::type*>
emitter::emitter::emit_for_cast(std::unique_ptr<ir::expr_ir> left, std::unique_ptr<ir::expr_ir> right) {
    if (left->expr_type == right->expr_type) {
        return {std::move(left), std::move(right), left->expr_type};
    }

    if (left->expr_type->can_be_implicitly_casted_to(right->expr_type)) {
        if (!left->expr_type->is_basic) {
            utils::log_error("Casting non-basic type is not supported for now.");
        }

        auto upcast_expr = emit_for_upcast(std::move(left), right->expr_type);

        return {std::move(upcast_expr), std::move(right), right->expr_type};
    }

    if (right->expr_type->can_be_implicitly_casted_to(left->expr_type)) {
        if (!right->expr_type->is_basic) {
            utils::log_error("Casting non-basic type is not supported for now.");
        }

        auto upcast_expr = emit_for_upcast(std::move(right), left->expr_type);

        return {std::move(left), std::move(upcast_expr), left->expr_type};
    }

    utils::log_error("Left and right side of expression cannot be explicitly casted to other side.");
    __builtin_unreachable();
}

std::unique_ptr<emitter::ir::upcast_expr_ir>
emitter::emitter::emit_for_upcast(std::unique_ptr<ir::expr_ir> inner_expr, ir::type *cast_to_type) {
    return std::make_unique<ir::upcast_expr_ir>(cast_to_type, std::move(inner_expr));
}

std::unique_ptr<emitter::ir::downcast_expr_ir>
emitter::emitter::emit_for_downcast(std::unique_ptr<ir::expr_ir> inner_expr, ir::type *cast_to_type) {
    return std::make_unique<ir::downcast_expr_ir>(cast_to_type, std::move(inner_expr));
}

std::unique_ptr<emitter::ir::identifier_expr_ir>
emitter::emitter::emit_for_identifier_expr(parser::ast::identifier_expr *identifier_expr) {
    auto argument_result = get_type_for_func_argument(identifier_expr->name);
    if (std::get<0>(argument_result) != nullptr) {
        return std::make_unique<ir::argument_exp_ir>(
                identifier_expr->name,
                std::get<0>(argument_result),
                std::get<1>(argument_result));
    }

    if (current_scope_->is_var_exists(identifier_expr->name)) {
        return std::make_unique<ir::identifier_expr_ir>(
                identifier_expr->name,
                false,
                current_scope_->get_type_for_variable(identifier_expr->name));
    }

    if (declared_global_variables_.contains(identifier_expr->name)) {
        return std::make_unique<ir::identifier_expr_ir>(
                identifier_expr->name,
                true,
                global_variables_types_[identifier_expr->name]);
    }

    auto generated_name = generate_func_static_var_name(identifier_expr->name);
    if (declared_global_variables_.contains(generated_name)) {
        return std::make_unique<ir::identifier_expr_ir>(
                generated_name,
                true,
                global_variables_types_[generated_name]);
    }

    utils::log_error(std::format("Undefined identifier: {}.", identifier_expr->name));
    __builtin_unreachable();
}

std::unique_ptr<emitter::ir::call_expr_ir> emitter::emitter::emit_for_call_expr(parser::ast::call_expr *call_expr) {
    if (!declared_functions_.contains(call_expr->name)) {
        utils::log_error(std::format("Attempted to call undefined function: {}.", call_expr->name));
    }

    auto func_type = functions_types_[call_expr->name];
    if (call_expr->arguments.size() != func_type->params.size()) {
        utils::log_error(std::format(
                "{} arguments was given, but function declaration has: {}.",
                call_expr->arguments.size(),
                func_type->params.size()));
    }

    std::vector<std::unique_ptr<ir::expr_ir>> arguments;
    for (int i = 0; i < call_expr->arguments.size(); i++) {
        auto argument_expr = emit_for_expr(std::move(call_expr->arguments[i]));
        auto param = func_type->params[i];

        arguments.push_back(emit_for_cast(std::move(argument_expr), param.param_type));
    }

    return std::make_unique<ir::call_expr_ir>(
            call_expr->name,
            std::move(arguments),
            functions_types_[call_expr->name]->return_type);
}

std::unique_ptr<emitter::ir::boolean_expr_ir>
emitter::emitter::emit_for_boolean_expr(parser::ast::boolean_expr *boolean_expr) {
    return std::make_unique<ir::boolean_expr_ir>(boolean_expr);
}

std::unique_ptr<emitter::ir::integer_expr_ir>
emitter::emitter::emit_for_integer_expr(parser::ast::integer_expr *integer_expr) {
    auto explicit_int_type = integer_expr->explicit_int_type;
    if (explicit_int_type.empty()) {
        return std::make_unique<ir::integer_expr_ir>(integer_expr);
    }

    return emit_for_explicitly_typed_integer(integer_expr, explicit_int_type);
}

std::unique_ptr<emitter::ir::integer_expr_ir> emitter::emitter::emit_for_explicitly_typed_integer(
        parser::ast::integer_expr* integer_expr, const std::string& explicit_int_type) {
    if (explicit_int_type == "int1") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::integer_type::int1());
    }

    if (explicit_int_type == "int8") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::integer_type::int8());
    }

    if (explicit_int_type == "int16") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::integer_type::int16());
    }

    if (explicit_int_type == "int32") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::integer_type::int32());
    }

    if (explicit_int_type == "int64") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::integer_type::int64());
    }

    if (explicit_int_type == "uint8") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::integer_type::uint8());
    }

    if (explicit_int_type == "uint16") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::integer_type::uint16());
    }

    if (explicit_int_type == "uint32") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::integer_type::uint32());
    }

    if (explicit_int_type == "uint") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::integer_type::uint32());
    }

    if (explicit_int_type == "uint64") {
        return std::make_unique<ir::integer_expr_ir>(integer_expr, ir::integer_type::uint64());
    }

    utils::log_error(std::format("Expected int type, but got: {} instead.", explicit_int_type));
    __builtin_unreachable();
}
