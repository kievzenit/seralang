#ifndef SNAKELANG_CALL_STMT_H
#define SNAKELANG_CALL_STMT_H

#include <utility>
#include <memory>
#include "stmt.h"
#include "call_expr.h"

namespace parser::ast {

    class call_stmt : public stmt {
    public:
        call_stmt(std::unique_ptr<call_expr> expression) : expression(std::move(expression)) {}

    public:
        std::unique_ptr<call_expr> expression;
    };

}

#endif //SNAKELANG_CALL_STMT_H
