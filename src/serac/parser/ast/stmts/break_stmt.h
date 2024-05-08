#ifndef SERALANG_BREAK_STMT_H
#define SERALANG_BREAK_STMT_H

#include "control_flow_stmt.h"

namespace parser::ast {

    class break_stmt : public control_flow_stmt {
    public:
        break_stmt() = default;
        explicit break_stmt(std::unique_ptr<expr> break_expr) : break_expr(std::move(break_expr)) {}

        std::unique_ptr<expr> break_expr;
    };

}

#endif //SERALANG_BREAK_STMT_H
