#ifndef SERALANG_COMPLEX_CAST_EXPR_H
#define SERALANG_COMPLEX_CAST_EXPR_H

#include <memory>
#include <string>

#include "expr.h"

namespace parser::ast {

    class complex_cast_expr : public expr {
    public:
        complex_cast_expr(std::unique_ptr<expr> left_expr, std::string cast_type, std::string new_identifier) :
            expr(left_expr->is_const),
            left_expr(std::move(left_expr)),
            cast_type(std::move(cast_type)),
            new_identifier(std::move(new_identifier)) {}

        std::string cast_type;
        std::string new_identifier;
        std::unique_ptr<expr> left_expr;
    };

}

#endif //SERALANG_COMPLEX_CAST_EXPR_H
