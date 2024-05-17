#include "emitter.h"

void emitter::emitter::emit_for_stmt(std::unique_ptr<parser::ast::stmt> stmt) {
    if (dynamic_cast<parser::ast::if_stmt*>(stmt.get()) != nullptr) {
        emit_for_if_stmt(dynamic_cast<parser::ast::if_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::while_stmt*>(stmt.get()) != nullptr) {
        emit_for_while_stmt(dynamic_cast<parser::ast::while_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::do_while_stmt*>(stmt.get()) != nullptr) {
        emit_for_do_while_stmt(dynamic_cast<parser::ast::do_while_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::loop_stmt*>(stmt.get()) != nullptr) {
        emit_for_loop_stmt(dynamic_cast<parser::ast::loop_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::for_stmt*>(stmt.get()) != nullptr) {
        emit_for_for_stmt(dynamic_cast<parser::ast::for_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::scope_stmt*>(stmt.get()) != nullptr) {
        auto scope =
                emit_for_scope_stmt(dynamic_cast<parser::ast::scope_stmt*>(stmt.get()));\
        current_scope_->inner_stmts.push_back(std::move(scope));
        return;
    }

    if (dynamic_cast<parser::ast::let_stmt*>(stmt.get()) != nullptr) {
        emit_for_let_stmt(dynamic_cast<parser::ast::let_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::expr_stmt*>(stmt.get()) != nullptr) {
        emit_for_expr_stmt(dynamic_cast<parser::ast::expr_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::return_stmt*>(stmt.get()) != nullptr) {
        emit_for_return_stmt(dynamic_cast<parser::ast::return_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::break_stmt*>(stmt.get()) != nullptr) {
        emit_for_break_stmt(dynamic_cast<parser::ast::break_stmt*>(stmt.get()));
        return;
    }

    if (dynamic_cast<parser::ast::breakall_stmt*>(stmt.get()) != nullptr) {
        emit_for_breakall_stmt();
        return;
    }

    if (dynamic_cast<parser::ast::continue_stmt*>(stmt.get()) != nullptr) {
        emit_for_continue_stmt();
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

        if (skip_all_statements_forward_) {
            skip_all_statements_forward_ = false;
            break;
        }
    }

    current_scope_ = current_scope_->parent_scope;

    return scope;
}

void emitter::emitter::emit_for_if_stmt(parser::ast::if_stmt *if_stmt) {
    auto if_expr = emit_for_cast(
            emit_for_expr(std::move(if_stmt->if_expr)),
            types_["bool"]);

    auto if_scope = emit_for_scope_stmt(if_stmt->scope.get());

    auto else_if_branches = emit_for_else_if_stmts(std::move(if_stmt->else_if_branches));

    std::unique_ptr<ir::else_stmt_ir> else_branch;
    if (if_stmt->else_branch) {
        else_branch = emit_for_else_stmt(if_stmt->else_branch.get());
    }

    auto if_stmt_ir = std::make_unique<ir::if_stmt_ir>(
            std::move(if_expr),
            std::move(if_scope),
            std::move(else_if_branches),
            std::move(else_branch));

    current_scope_->inner_stmts.push_back(std::move(if_stmt_ir));
}

std::vector<std::unique_ptr<emitter::ir::else_if_stmt_ir>>
emitter::emitter::emit_for_else_if_stmts(std::vector<std::unique_ptr<parser::ast::else_if_stmt>> else_if_branches) {
    std::vector<std::unique_ptr<ir::else_if_stmt_ir>> else_if_ir_branches;

    for (auto& else_if : else_if_branches) {
        auto else_if_stmt = emit_for_else_if_stmt(else_if.get());
        else_if_ir_branches.push_back(std::move(else_if_stmt));
    }

    return else_if_ir_branches;
}

std::unique_ptr<emitter::ir::else_if_stmt_ir>
emitter::emitter::emit_for_else_if_stmt(parser::ast::else_if_stmt *else_if_stmt) {
    auto else_if_expr = emit_for_cast(
            emit_for_expr(std::move(else_if_stmt->if_expr)),
            types_["bool"]);

    auto else_if_scope = emit_for_scope_stmt(else_if_stmt->scope.get());

    return std::make_unique<ir::else_if_stmt_ir>(std::move(else_if_expr), std::move(else_if_scope));
}

std::unique_ptr<emitter::ir::else_stmt_ir> emitter::emitter::emit_for_else_stmt(parser::ast::else_stmt *else_stmt) {
    auto else_scope = emit_for_scope_stmt(else_stmt->scope.get());
    return std::make_unique<ir::else_stmt_ir>(std::move(else_scope));
}

void emitter::emitter::emit_for_while_stmt(parser::ast::while_stmt *while_stmt) {
    loop_count_++;

    auto condition_expr = emit_for_cast(
            emit_for_expr(std::move(while_stmt->condition)),
            types_["bool"]);

    auto scope = emit_for_scope_stmt(while_stmt->scope.get());

    auto while_stmt_ir = std::make_unique<ir::while_stmt_ir>(
            std::move(condition_expr),
            std::move(scope));

    current_scope_->inner_stmts.push_back(std::move(while_stmt_ir));

    loop_count_--;
}

void emitter::emitter::emit_for_do_while_stmt(parser::ast::do_while_stmt *do_while_stmt) {
    loop_count_++;

    auto scope = emit_for_scope_stmt(do_while_stmt->scope.get());

    auto condition_expr = emit_for_cast(
            emit_for_expr(std::move(do_while_stmt->condition)),
            types_["bool"]);

    auto while_stmt_ir = std::make_unique<ir::do_while_stmt_ir>(
            std::move(condition_expr),
            std::move(scope));

    current_scope_->inner_stmts.push_back(std::move(while_stmt_ir));

    loop_count_--;
}

void emitter::emitter::emit_for_loop_stmt(parser::ast::loop_stmt *loop_stmt) {
    loop_count_++;

    auto scope = emit_for_scope_stmt(loop_stmt->scope.get());
    auto loop_stmt_ir = std::make_unique<ir::loop_stmt_ir>(std::move(scope));

    current_scope_->inner_stmts.push_back(std::move(loop_stmt_ir));

    loop_count_--;
}

void emitter::emitter::emit_for_for_stmt(parser::ast::for_stmt* for_stmt) {
    loop_count_++;

    auto for_scope = std::make_unique<ir::scope_stmt_ir>(current_scope_);
    auto for_scope_ptr = for_scope.get();
    current_scope_->inner_stmts.push_back(std::move(for_scope));
    current_scope_ = for_scope_ptr;

    for (auto &run_once: for_stmt->run_once) {
        if (dynamic_cast<parser::ast::let_stmt *>(run_once.get()) != nullptr) {
            auto let_stmt = dynamic_cast<parser::ast::let_stmt *>(run_once.get());
            if (let_stmt->is_static) {
                utils::log_error("Static let variables cannot be use inside for init stage.");
            }

            emit_for_let_stmt(let_stmt);
            continue;
        }

        if (dynamic_cast<parser::ast::expr_stmt *>(run_once.get()) != nullptr) {
            emit_for_expr_stmt(dynamic_cast<parser::ast::expr_stmt*>(run_once.get()));
            continue;
        }

        utils::log_error("Only let and  expression statements are allowed in init stage of for loop.");
    }

    auto condition = emit_for_cast(
            emit_for_expr(std::move(for_stmt->condition)),
            types_["bool"]);

    auto scope = emit_for_scope_stmt(for_stmt->scope.get());
    current_scope_ = scope.get();

    auto while_stmt = std::make_unique<ir::while_stmt_ir>(std::move(condition), std::move(scope));

    auto run_after_each_scope = std::make_unique<ir::scope_stmt_ir>(
            while_stmt->scope.get());
    auto run_after_each_scope_ptr = run_after_each_scope.get();
    current_scope_->inner_stmts.push_back(std::move(run_after_each_scope));
    current_scope_ = run_after_each_scope_ptr;

    for (auto &run_after_each: for_stmt->run_after_each) {
        auto expr = emit_for_expr(std::move(run_after_each));
        auto stmt_expr = std::make_unique<ir::expr_stmt_ir>(std::move(expr));
        current_scope_->inner_stmts.push_back(std::move(stmt_expr));
    }

    for_scope_ptr->inner_stmts.push_back(std::move(while_stmt));
    current_scope_ = for_scope_ptr->parent_scope;

    loop_count_--;
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

    if (let_stmt->is_static && !let_stmt->expression->is_const) {
        utils::log_error("Static variables could only be instantiated with const expressions.");
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

void emitter::emitter::emit_for_expr_stmt(parser::ast::expr_stmt *expr_stmt) {
    auto expr_ir = emit_for_expr(std::move(expr_stmt->inner_expr));
    auto expr_stmt_ir = std::make_unique<ir::expr_stmt_ir>(std::move(expr_ir));

    current_scope_->inner_stmts.push_back(std::move(expr_stmt_ir));
}

void emitter::emitter::emit_for_return_stmt(parser::ast::return_stmt *return_stmt) {
    auto return_expr = emit_for_expr(std::move(return_stmt->return_expr));
    auto cast_result = emit_for_cast(
            std::move(return_expr), current_function_->return_type);

    auto return_stmt_ir = std::make_unique<ir::return_stmt_ir>(std::move(cast_result));
    current_scope_->inner_stmts.push_back(std::move(return_stmt_ir));

    skip_all_statements_forward_ = true;
}

void emitter::emitter::emit_for_break_stmt(parser::ast::break_stmt* break_stmt) {
    if (loop_count_ == -1) {
        utils::log_error("Break can be placed only inside loop.");
    }

    std::unique_ptr<ir::expr_ir> break_expr;
    if (break_stmt->break_expr) {
        break_expr = emit_for_expr(std::move(break_stmt->break_expr));
        if (dynamic_cast<ir::integer_type *>(break_expr->expr_type) == nullptr) {
            utils::log_error("Only expressions that returns integer are allowed to be after break statement.");
        }

        auto integer_type = dynamic_cast<ir::integer_type *>(break_expr->expr_type);
        if (!integer_type->is_unsigned) {
            utils::log_error("Only unsigned integers are allowed after break statement.");
        }
    }

    auto break_stmt_ir = std::make_unique<ir::break_stmt_ir>(std::move(break_expr));
    current_scope_->inner_stmts.push_back(std::move(break_stmt_ir));

    skip_all_statements_forward_ = true;
}

void emitter::emitter::emit_for_breakall_stmt() {
    if (loop_count_ == -1) {
        utils::log_error("Breakall can be placed only inside loop.");
    }

    auto breakall_stmt_ir = std::make_unique<ir::breakall_stmt_ir>();
    current_scope_->inner_stmts.push_back(std::move(breakall_stmt_ir));

    skip_all_statements_forward_ = true;
}

void emitter::emitter::emit_for_continue_stmt() {
    if (loop_count_ == -1) {
        utils::log_error("Breakall can be placed only inside loop.");
    }

    auto continue_stmt_ir = std::make_unique<ir::continue_stmt_ir>();
    current_scope_->inner_stmts.push_back(std::move(continue_stmt_ir));

    skip_all_statements_forward_ = true;
}
