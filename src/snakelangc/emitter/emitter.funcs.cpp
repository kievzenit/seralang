#include "emitter.h"

bool emitter::emitter::is_identifier_is_func_argument(const std::string &name) {
    for (auto& param : current_function_->params) {
        if (param.name == name) {
            return true;
        }
    }

    return false;
}

std::string emitter::emitter::generate_func_static_var_name(const std::string &var_name) {
    return std::format("{}::{}", current_function_->name, var_name);
}

std::tuple<emitter::ir::type*, int> emitter::emitter::get_type_for_func_argument(const std::string &name) {
    for (int i = 0; i < current_function_->params.size(); i++) {
        auto param = current_function_->params[i];
        if (param.name == name) {
            return {param.param_type, i};
        }
    }

    return {nullptr, -1};
}

void emitter::emitter::emit_all_func_declarations() {
    for (auto &top_stmt : translation_asts_[0]->stmts) {
        if (dynamic_cast<parser::ast::func_decl_stmt*>(top_stmt.get()) == nullptr) {
            continue;
        }

        auto func_decl_stmt =dynamic_cast<parser::ast::func_decl_stmt*>(top_stmt.get());
        current_function_ = functions_types_[func_decl_stmt->name];
        functions_.push_back(emit_for_func(func_decl_stmt));
        current_function_ = nullptr;
    }
}

std::unique_ptr<emitter::ir::func_decl_ir> emitter::emitter::emit_for_func(parser::ast::func_decl_stmt *func_stmt) {
    auto root_scope_stmt_ir = emit_for_scope_stmt(func_stmt->func_scope.get());
    auto last_stmt_ir = root_scope_stmt_ir->inner_stmts.back().get();
    if (dynamic_cast<ir::return_stmt_ir*>(last_stmt_ir) == nullptr) {
        utils::log_error("Expected return in the end of function, but got nothing instead.");
        __builtin_unreachable();
    }

    return std::make_unique<ir::func_decl_ir>(
            func_stmt->name,
            current_function_->params,
            current_function_->return_type,
            std::move(root_scope_stmt_ir));
}

std::vector<emitter::ir::func_param_ir> emitter::emitter::emit_func_params(parser::ast::func_decl_stmt *func_stmt) {
    std::vector<ir::func_param_ir> params;

    for (const auto& param : func_stmt->params) {
        auto func_param = emit_for_func_param(param);
        params.push_back(func_param);
    }

    return params;
}

emitter::ir::func_param_ir emitter::emitter::emit_for_func_param(const parser::ast::func_param& func_param) {
    auto type = types_[func_param.type];
    if (type == nullptr) {
        utils::log_error(std::format("Undefined type: {}.", func_param.type));
    }

    return {func_param.name, type};
}
