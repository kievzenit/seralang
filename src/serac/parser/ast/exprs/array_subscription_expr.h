#ifndef SERALANG_ARRAY_SUBSCRIPTION_EXPR_H
#define SERALANG_ARRAY_SUBSCRIPTION_EXPR_H

#include <memory>
#include "expr.h"

namespace parser::ast {

    class array_subscription_expr : public expr {
    public:
        array_subscription_expr(std::unique_ptr<expr> left_expr, std::unique_ptr<expr> subscription_expr) :
                expr(false),
                left_expr(std::move(left_expr)),
                subscription_expr(std::move(subscription_expr)) {}

        std::unique_ptr<expr> left_expr;
        std::unique_ptr<expr> subscription_expr;
    };

}

#endif //SERALANG_ARRAY_SUBSCRIPTION_EXPR_H
