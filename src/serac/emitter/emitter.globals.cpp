#include "emitter.h"

void emitter::emitter::emit_all_global_variables() {
    for (auto &top_stmt : translation_asts_[0]->stmts) {
        if (dynamic_cast<parser::ast::let_stmt *>(top_stmt.get()) == nullptr) {
            continue;
        }

        auto let_stmt = dynamic_cast<parser::ast::let_stmt *>(top_stmt.get());

        if (!let_stmt->is_static) {
            auto error = std::make_unique<errors::error>(
                    "Top let statement must be static.",
                    "add static keyword before let.",
                    top_stmt->metadata.file_name,
                    top_stmt->metadata.line_start,
                    top_stmt->metadata.column_start,
                    top_stmt->metadata.column_end);
            errors.push_back(std::move(error));
            continue;
        }

        if (!let_stmt->expression->is_const) {
            auto error = std::make_unique<errors::error>(
                    "Static global variables can only be instantiated with const expressions.",
                    "use constant expression (can be calculated in compile time) as assignment to variable.",
                    top_stmt->metadata.file_name,
                    let_stmt->expression->metadata.line_start,
                    let_stmt->expression->metadata.column_start,
                    let_stmt->expression->metadata.column_end);
            errors.push_back(std::move(error));
            continue;
        }

        auto expr_ir = emit_for_expr(std::move(let_stmt->expression));
        global_variables_types_[let_stmt->name] = expr_ir->expr_type;
        auto variable_ir = std::make_unique<ir::let_stmt_ir>(
                let_stmt->name, std::move(expr_ir), expr_ir->expr_type, true, false);

        global_variables_.push_back(std::move(variable_ir));
    }
}
