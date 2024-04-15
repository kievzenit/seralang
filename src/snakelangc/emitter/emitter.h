#ifndef SNAKELANG_EMITTER_H
#define SNAKELANG_EMITTER_H

#include <memory>

#include "../utils/log_error.h"
#include "../parser/ast/translation_ast.h"
#include "../parser/ast/let_stmt.h"
#include "../parser/ast/binary_expr.h"
#include "ir/package_ir.h"
#include "ir/binary_expr_ir.h"
#include "ir/variable_ir.h"
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

        std::vector<std::unique_ptr<ir::variable_ir>> find_all_global_variables();

        std::unique_ptr<ir::expr_ir> emit_for_expr(std::unique_ptr<parser::ast::expr> expr);
        std::unique_ptr<ir::integer_expr_ir> emit_for_specific_integer(
                parser::ast::integer_expr* integer_expr,
                const std::string& explicit_int_type);
    };

}

#endif //SNAKELANG_EMITTER_H
