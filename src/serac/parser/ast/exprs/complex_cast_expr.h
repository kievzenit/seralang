#ifndef SERALANG_COMPLEX_CAST_EXPR_H
#define SERALANG_COMPLEX_CAST_EXPR_H

#include <memory>
#include <string>
#include <utility>

#include "cast_expr.h"

namespace parser::ast {

    class complex_cast_expr : public cast_expr {
    public:
        complex_cast_expr(std::unique_ptr<expr> left_expr, std::string cast_type, std::string new_identifier) :
            cast_expr(std::move(left_expr), std::move(cast_type)),
            new_identifier(std::move(new_identifier)) {}

        std::string new_identifier;
    };

}

#endif //SERALANG_COMPLEX_CAST_EXPR_H
