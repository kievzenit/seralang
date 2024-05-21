#ifndef SERALANG_CAST_EXPR_H
#define SERALANG_CAST_EXPR_H

#include <memory>
#include <string>

#include "expr.h"

namespace parser::ast {

    class cast_expr : public expr {
    public:
        explicit cast_expr(std::unique_ptr<expr> left_expr, std::string cast_type) :
            expr(left_expr->is_const),
            left_expr(std::move(left_expr)),
            cast_type(std::move(cast_type)) {}

        std::unique_ptr<expr> left_expr;
        std::string cast_type;
    };

}

#endif //SERALANG_CAST_EXPR_H
