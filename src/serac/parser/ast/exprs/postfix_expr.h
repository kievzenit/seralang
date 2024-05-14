#ifndef SERALANG_POSTFIX_EXPR_H
#define SERALANG_POSTFIX_EXPR_H

#include "unary_expr.h"

namespace parser::ast {

    class postfix_expr : public unary_expr {
    public:
        postfix_expr(std::unique_ptr<expr> inner_expr, unary_operation operation) :
                unary_expr(std::move(inner_expr), operation) {}
    };

}

#endif //SERALANG_POSTFIX_EXPR_H
