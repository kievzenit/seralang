#include "emitter.h"

void emitter::emitter::emit_for_stmt(std::unique_ptr<parser::ast::stmt> stmt) {
    if (dynamic_cast<parser::ast::scope_stmt*>(stmt.get()) != nullptr) {
        emit_for_scope_stmt(dynamic_cast<parser::ast::scope_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::let_stmt*>(stmt.get()) != nullptr) {
        emit_for_let_stmt(dynamic_cast<parser::ast::let_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::assignment_stmt*>(stmt.get()) != nullptr) {
        emit_for_assignment_stmt(dynamic_cast<parser::ast::assignment_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::call_stmt*>(stmt.get()) != nullptr) {
        emit_for_call_stmt(dynamic_cast<parser::ast::call_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::return_stmt*>(stmt.get()) != nullptr) {
        emit_for_return_stmt(dynamic_cast<parser::ast::return_stmt*>(stmt.get()));
        return;
    }

    utils::log_error("Unsupported statement encountered, this should never happen!");
    __builtin_unreachable();
}

std::unique_ptr<emitter::ir::scope_stmt_ir>
emitter::emitter::emit_for_scope_stmt(parser::ast::scope_stmt* scope_stmt) {
    auto scope = std::make_unique<ir::scope_stmt_ir>(current_scope_);
    current_scope_ = scope.get();

    for (auto &stmt : scope_stmt->inner_stmts) {
        emit_for_stmt(std::move(stmt));
    }

    current_scope_ = current_scope_->parent_scope;

    return scope;
}

void emitter::emitter::emit_for_let_stmt(parser::ast::let_stmt *let_stmt) {
    if (is_identifier_is_func_argument(let_stmt->name)) {
        utils::log_error(
                std::format(
                        "Variable with name: {}, is already defined as function parameter.",
                        let_stmt->name));
    }

    if (current_scope_->static_variables_types.contains(let_stmt->name)) {
        utils::log_error(
                std::format(
                        "Variable with name: {}, is already defined in current scope as a static variable.",
                        let_stmt->name));
    }

    auto expr_ir = emit_for_expr(std::move(let_stmt->expression));

    if (let_stmt->is_static) {
        auto generated_name = generate_func_static_var_name(let_stmt->name);
        declared_global_variables_.insert(generated_name);
        global_variables_types_.insert(std::make_pair(generated_name, expr_ir->expr_type));
        current_scope_->static_variables_types.insert(std::make_pair(let_stmt->name, expr_ir->expr_type));

        auto variable_ir = std::make_unique<ir::variable_ir>(
                generated_name,
                std::move(expr_ir),
                expr_ir->expr_type,
                true,
                true);
        global_variables_.push_back(std::move(variable_ir));
        return;
    }

    if (current_scope_->variables_types.contains(let_stmt->name)) {
        utils::log_error(
                std::format(
                        "Variable with name: {}, is already defined in current scope.",
                        let_stmt->name));
    }

    current_scope_->variables_types.insert(std::make_pair(let_stmt->name, expr_ir->expr_type));
    auto variable_ir = std::make_unique<ir::variable_ir>(
            let_stmt->name,
            std::move(expr_ir),
            expr_ir->expr_type,
            false,
            true);
    current_scope_->inner_stmts.push_back(std::move(variable_ir));
}

void emitter::emitter::emit_for_assignment_stmt(parser::ast::assignment_stmt *assignment_stmt) {
    auto assignment_expr = emit_for_expr(std::move(assignment_stmt->assignment_expr));

    if (current_scope_->is_var_exists(assignment_stmt->name)) {
        auto identifier_type = current_scope_->variables_types[assignment_stmt->name];
        auto assignment_stmt_ir = std::make_unique<ir::assignment_stmt_ir>(
                assignment_stmt->name,
                false,
                emit_for_cast(std::move(assignment_expr), identifier_type));
        current_scope_->inner_stmts.push_back(std::move(assignment_stmt_ir));
        return;
    }

    if (declared_global_variables_.contains(assignment_stmt->name)) {
        auto global_var_type = global_variables_types_[assignment_stmt->name];
        auto assignment_stmt_ir = std::make_unique<ir::assignment_stmt_ir>(
                assignment_stmt->name,
                true,
                emit_for_cast(std::move(assignment_expr), global_var_type));
        current_scope_->inner_stmts.push_back(std::move(assignment_stmt_ir));
        return;
    }

    auto generated_name = generate_func_static_var_name(assignment_stmt->name);
    if (declared_global_variables_.contains(generated_name)) {
        auto global_var_type = global_variables_types_[generated_name];
        auto assignment_stmt_ir = std::make_unique<ir::assignment_stmt_ir>(
                generated_name,
                true,
                emit_for_cast(std::move(assignment_expr), global_var_type));
        current_scope_->inner_stmts.push_back(std::move(assignment_stmt_ir));
        return;
    }

    utils::log_error(std::format("Undefined identifier: {}.", assignment_stmt->name));
    __builtin_unreachable();
}

void emitter::emitter::emit_for_call_stmt(parser::ast::call_stmt *call_stmt) {
    auto call_stmt_ir = std::make_unique<ir::call_stmt_ir>(emit_for_call_expr(call_stmt->expression.get()));
    current_scope_->inner_stmts.push_back(std::move(call_stmt_ir));
}

void emitter::emitter::emit_for_return_stmt(parser::ast::return_stmt *return_stmt) {
    auto return_expr = emit_for_expr(std::move(return_stmt->return_expr));
    auto cast_result = emit_for_cast(
            std::move(return_expr), current_function_->return_type);

    auto return_stmt_ir = std::make_unique<ir::return_stmt_ir>(std::move(cast_result));
    current_scope_->inner_stmts.push_back(std::move(return_stmt_ir));
}
