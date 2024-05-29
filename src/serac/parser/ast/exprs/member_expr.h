#ifndef SERALANG_MEMBER_EXPR_H
#define SERALANG_MEMBER_EXPR_H

#include <vector>
#include <memory>

#include "expr.h"

namespace parser::ast {

    class member_expr : public expr {
    public:
        member_expr(std::unique_ptr<expr> left_expr, std::unique_ptr<expr> right_expr) :
                expr(false),
                left_expr(std::move(left_expr)), right_expr(std::move(right_expr)) {}

        std::unique_ptr<expr> left_expr;
        std::unique_ptr<expr> right_expr;
    };

}

#endif //SERALANG_MEMBER_EXPR_H
