#ifndef SERALANG_PREFIX_EXPR_H
#define SERALANG_PREFIX_EXPR_H

#include "unary_expr.h"

namespace parser::ast {

    class prefix_expr : public unary_expr {
    public:
        prefix_expr(std::unique_ptr<expr> inner_expr, unary_operation operation) :
                unary_expr(std::move(inner_expr), operation) {}
    };

}


#endif //SERALANG_PREFIX_EXPR_H
