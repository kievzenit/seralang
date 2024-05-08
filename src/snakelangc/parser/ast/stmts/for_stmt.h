#ifndef SNAKELANG_FOR_STMT_H
#define SNAKELANG_FOR_STMT_H

#include <memory>
#include <vector>

#include "stmt.h"
#include "../exprs/expr.h"

namespace parser::ast {

    class for_stmt : public stmt {
    public:
        for_stmt(
                std::vector<std::unique_ptr<stmt>> run_once,
                std::unique_ptr<expr> condition,
                std::vector<std::unique_ptr<stmt>> run_after_each,
                std::unique_ptr<scope_stmt> scope) :
                run_once(std::move(run_once)),
                condition(std::move(condition)),
                run_after_each(std::move(run_after_each)),
                scope(std::move(scope)) {}

        std::vector<std::unique_ptr<stmt>> run_once;
        std::unique_ptr<expr> condition;
        std::vector<std::unique_ptr<stmt>> run_after_each;
        std::unique_ptr<scope_stmt> scope;
    };

}

#endif //SNAKELANG_FOR_STMT_H