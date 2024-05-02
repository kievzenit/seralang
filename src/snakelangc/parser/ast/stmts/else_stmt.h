#ifndef SNAKELANG_ELSE_STMT_H
#define SNAKELANG_ELSE_STMT_H

#include "stmt.h"
#include "scope_stmt.h"

namespace parser::ast {

    class else_stmt : public stmt {
    public:
        explicit else_stmt(std::unique_ptr<scope_stmt> scope) : scope(std::move(scope)) {}

        std::unique_ptr<scope_stmt> scope;
    };

}

#endif //SNAKELANG_ELSE_STMT_H
