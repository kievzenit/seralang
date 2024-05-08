#ifndef SNAKELANG_LOOP_STMT_H
#define SNAKELANG_LOOP_STMT_H

#include <memory>
#include "compound_stmt.h"
#include "scope_stmt.h"

namespace parser::ast {

    class loop_stmt : public compound_stmt {
    public:
        explicit loop_stmt(std::unique_ptr<scope_stmt> scope) : scope(std::move(scope)) {}

        std::unique_ptr<scope_stmt> scope;
    };

}

#endif //SNAKELANG_LOOP_STMT_H
