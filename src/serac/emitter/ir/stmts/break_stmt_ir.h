#ifndef SERALANG_BREAK_STMT_IR_H
#define SERALANG_BREAK_STMT_IR_H

#include "stmt_ir.h"

namespace emitter::ir {

    class break_stmt_ir : public stmt_ir {
    public:
        explicit break_stmt_ir(std::unique_ptr<expr_ir> break_expr) : break_expr(std::move(break_expr)) {}

        std::unique_ptr<expr_ir> break_expr;
    };

}

#endif //SERALANG_BREAK_STMT_IR_H
