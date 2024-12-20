#include "emitter.h"

std::unique_ptr<emitter::ir::package_ir> emitter::emitter::emit() {
    find_globals();

    emit_all_global_variables();
    emit_all_func_declarations();

    return std::make_unique<ir::package_ir>(
            translation_asts_[0]->package->package_name,
            std::move(global_variables_),
            std::move(functions_));
}

void emitter::emitter::find_globals() {
    for (auto &top_stmt : translation_asts_[0]->stmts) {
        if (dynamic_cast<parser::ast::let_stmt *>(top_stmt.get()) != nullptr) {
            auto let_stmt = dynamic_cast<parser::ast::let_stmt *>(top_stmt.get());

            auto insertion_result = declared_global_variables_.insert(let_stmt->name);
            if (!insertion_result.second) {
                auto error = std::make_unique<errors::error>(
                        std::format(
                                "Global variable with name: \"{}\" is already defined.",
                                let_stmt->name),
                        "try rename variable.",
                        top_stmt->metadata.file_name,
                        top_stmt->metadata.line_start,
                        top_stmt->metadata.column_start,
                        top_stmt->metadata.column_end);
                errors.push_back(std::move(error));
            }

            continue;
        }

        if (dynamic_cast<parser::ast::func_decl_stmt *>(top_stmt.get()) != nullptr) {
            auto func_stmt = dynamic_cast<parser::ast::func_decl_stmt *>(top_stmt.get());

            auto insertion_result = declared_functions_.insert(func_stmt->name);
            if (!insertion_result.second) {
                auto error = std::make_unique<errors::error>(
                        std::format(
                                "Function with name: \"{}\" is already defined.",
                                func_stmt->name),
                        "try rename function.",
                        top_stmt->metadata.file_name,
                        top_stmt->metadata.line_start,
                        top_stmt->metadata.column_start,
                        top_stmt->metadata.column_end);
                errors.push_back(std::move(error));

                continue;
            }

            auto params = emit_func_params(func_stmt);

            auto return_type = types_[func_stmt->return_type];
            if (return_type == nullptr) {
                auto error = std::make_unique<errors::error>(
                        std::format(
                                "Undefined type: \"{}\".",
                                func_stmt->return_type),
                        "use type that is defined.",
                        top_stmt->metadata.file_name,
                        top_stmt->metadata.line_start,
                        top_stmt->metadata.column_start,
                        top_stmt->metadata.column_end);
                errors.push_back(std::move(error));
                continue;
            }
            functions_types_[func_stmt->name] = new ir::func_type(func_stmt->name, params, return_type);

            continue;
        }
    }
}
