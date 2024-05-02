#ifndef SNAKELANG_IF_STMT_H
#define SNAKELANG_IF_STMT_H

#include <memory>
#include <vector>
#include "stmt.h"
#include "../exprs/expr.h"
#include "scope_stmt.h"
#include "else_if_stmt.h"
#include "else_stmt.h"

namespace parser::ast {

    class if_stmt : public stmt {
    public:
        if_stmt(
                std::unique_ptr<expr> if_expr,
                std::unique_ptr<scope_stmt> scope,
                std::vector<std::unique_ptr<else_if_stmt>> else_if_branches,
                std::unique_ptr<else_stmt> else_branch) :
                if_expr(std::move(if_expr)),
                scope(std::move(scope)),
                else_if_branches(std::move(else_if_branches)),
                else_branch(std::move(else_branch)) {}

        std::unique_ptr<expr> if_expr;
        std::unique_ptr<scope_stmt> scope;

        std::vector<std::unique_ptr<else_if_stmt>> else_if_branches;

        std::unique_ptr<else_stmt> else_branch;
    };

}

#endif //SNAKELANG_IF_STMT_H
