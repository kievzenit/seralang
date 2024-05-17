#ifndef SERALANG_EXPR_STMT_H
#define SERALANG_EXPR_STMT_H

#include <memory>

#include "local_stmt.h"
#include "../exprs/expr.h"

namespace parser::ast {

    class expr_stmt : public local_stmt {
    public:
        explicit expr_stmt(std::unique_ptr<expr> inner_expr) :
            inner_expr(std::move(inner_expr)) {}

        std::unique_ptr<expr> inner_expr;
    };

}

#endif //SERALANG_EXPR_STMT_H
