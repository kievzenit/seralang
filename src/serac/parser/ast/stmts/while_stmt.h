#ifndef SERALANG_WHILE_STMT_H
#define SERALANG_WHILE_STMT_H

#include <memory>
#include "compound_stmt.h"
#include "../exprs/expr.h"
#include "scope_stmt.h"

namespace parser::ast {

    class while_stmt : public compound_stmt {
    public:
        while_stmt(std::unique_ptr<expr> condition, std::unique_ptr<scope_stmt> scope) :
            condition(std::move(condition)), scope(std::move(scope)) {}

        std::unique_ptr<expr> condition;
        std::unique_ptr<scope_stmt> scope;
    };

}

#endif //SERALANG_WHILE_STMT_H
