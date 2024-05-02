#ifndef SNAKELANG_ELSE_IF_STMT_H
#define SNAKELANG_ELSE_IF_STMT_H

#include <memory>
#include "stmt.h"
#include "../exprs/expr.h"
#include "scope_stmt.h"

namespace parser::ast {

    class else_if_stmt : public stmt {
    public:
        else_if_stmt(std::unique_ptr<expr> if_expr, std::unique_ptr<scope_stmt> scope) :
            if_expr(std::move(if_expr)), scope(std::move(scope)) {}

        std::unique_ptr<expr> if_expr;
        std::unique_ptr<scope_stmt> scope;
    };

}

#endif //SNAKELANG_ELSE_IF_STMT_H
