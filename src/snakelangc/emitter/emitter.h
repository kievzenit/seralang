#ifndef SNAKELANG_EMITTER_H
#define SNAKELANG_EMITTER_H

#include <memory>
#include <algorithm>
#include <map>

#include "../utils/log_error.h"
#include "../parser/ast/translation_ast.h"
#include "../parser/ast/let_stmt.h"
#include "../parser/ast/return_stmt.h"
#include "../parser/ast/func_decl_stmt.h"
#include "../parser/ast/binary_expr.h"
#include "ir/package_ir.h"
#include "ir/binary_expr_ir.h"
#include "ir/variable_ir.h"
#include "ir/return_ir.h"
#include "ir/integer_expr_ir.h"
#include "ir/boolean_expr_ir.h"

namespace emitter {

    class emitter {
    public:
        emitter(std::unique_ptr<parser::ast::translation_ast> translation_ast) :
            translation_ast_(std::move(translation_ast)) {}

            std::unique_ptr<ir::package_ir> emit();

    private:
        std::unique_ptr<parser::ast::translation_ast> translation_ast_;

        std::vector<std::string> global_variables_;
        std::vector<std::string> functions_;
        std::map<std::string, ir::type> types_ = {
                {"bool", ir::type::boolean()},
                {"int1", ir::type::int1()},
                {"int8", ir::type::int8()},
                {"int16", ir::type::int16()},
                {"int32", ir::type::int32()},
                {"int", ir::type::int32()},
                {"int64", ir::type::int64()},
                {"uint8", ir::type::uint8()},
                {"uint16", ir::type::uint16()},
                {"uint32", ir::type::uint32()},
                {"uint64", ir::type::uint64()}
        };

        bool is_global_variable_exists(const std::string& name);
        bool is_function_declared(const std::string& name);

        std::vector<std::unique_ptr<ir::variable_ir>> find_all_global_variables();
        std::vector<std::unique_ptr<ir::func_decl_ir>> find_all_func_declarations();

        std::unique_ptr<ir::scope_stmt_ir> emit_for_scope_stmt(parser::ast::scope_stmt* scope_stmt);
        std::unique_ptr<ir::stmt_ir> emit_for_stmt(std::unique_ptr<parser::ast::stmt> stmt);

        std::unique_ptr<ir::expr_ir> emit_for_expr(std::unique_ptr<parser::ast::expr> expr);
        static std::unique_ptr<ir::integer_expr_ir> emit_for_explicitly_typed_integer(
                parser::ast::integer_expr* integer_expr,
                const std::string& explicit_int_type);
    };

}

#endif //SNAKELANG_EMITTER_H
