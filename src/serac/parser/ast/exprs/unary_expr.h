#ifndef SERALANG_UNARY_EXPR_H
#define SERALANG_UNARY_EXPR_H

#include <memory>

#include "expr.h"
#include "../common/unary_operation.h"

namespace parser::ast {

    class unary_expr : public expr {
    public:
        unary_expr(std::unique_ptr<expr> inner_expr, unary_operation operation) :
                expr(inner_expr->is_const), inner_expr(std::move(inner_expr)), operation(operation) {}

        std::unique_ptr<expr> inner_expr;
        unary_operation operation;
    };

}

#endif //SERALANG_UNARY_EXPR_H
