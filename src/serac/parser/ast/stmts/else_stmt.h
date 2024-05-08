#ifndef SERALANG_ELSE_STMT_H
#define SERALANG_ELSE_STMT_H

#include "compound_stmt.h"
#include "scope_stmt.h"

namespace parser::ast {

    class else_stmt : public compound_stmt {
    public:
        explicit else_stmt(std::unique_ptr<scope_stmt> scope) : scope(std::move(scope)) {}

        std::unique_ptr<scope_stmt> scope;
    };

}

#endif //SERALANG_ELSE_STMT_H
