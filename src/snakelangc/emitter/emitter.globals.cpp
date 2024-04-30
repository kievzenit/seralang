#include "emitter.h"

void emitter::emitter::emit_all_global_variables() {
    for (auto &top_stmt : translation_asts_[0]->stmts) {
        if (dynamic_cast<parser::ast::let_stmt *>(top_stmt.get()) == nullptr) {
            continue;
        }

        auto let_stmt = dynamic_cast<parser::ast::let_stmt *>(top_stmt.get());

        if (!let_stmt->is_static) {
            utils::log_error("Top let statement must be static.");
        }

        if (!let_stmt->expression->is_const) {
            utils::log_error("Static global variables can only be instantiated with const expressions.");
        }

        auto expr_ir = emit_for_expr(std::move(let_stmt->expression));
        global_variables_types_[let_stmt->name] = expr_ir->expr_type;
        auto variable_ir = std::make_unique<ir::variable_ir>(
                let_stmt->name, std::move(expr_ir), expr_ir->expr_type, true, false);

        global_variables_.push_back(std::move(variable_ir));
    }
}
