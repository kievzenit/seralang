#ifndef SNAKELANG_DO_WHILE_STMT_H
#define SNAKELANG_DO_WHILE_STMT_H

#include <memory>
#include "stmt.h"
#include "../exprs/expr.h"
#include "scope_stmt.h"

namespace parser::ast {

    class do_while_stmt : public stmt {
    public:
        do_while_stmt(std::unique_ptr<expr> condition, std::unique_ptr<scope_stmt> scope) :
                condition(std::move(condition)), scope(std::move(scope)) {}

        std::unique_ptr<expr> condition;
        std::unique_ptr<scope_stmt> scope;
    };

}

#endif //SNAKELANG_DO_WHILE_STMT_H
