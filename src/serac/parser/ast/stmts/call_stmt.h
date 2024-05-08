#ifndef SERALANG_CALL_STMT_H
#define SERALANG_CALL_STMT_H

#include <utility>
#include <memory>
#include "local_stmt.h"
#include "../exprs/call_expr.h"

namespace parser::ast {

    class call_stmt : public local_stmt {
    public:
        call_stmt(std::unique_ptr<call_expr> expression) : expression(std::move(expression)) {}

    public:
        std::unique_ptr<call_expr> expression;
    };

}

#endif //SERALANG_CALL_STMT_H
