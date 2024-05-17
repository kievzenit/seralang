#ifndef SERALANG_EXPR_STMT_IR_H
#define SERALANG_EXPR_STMT_IR_H

#include "stmt_ir.h"
#include "../exprs/expr_ir.h"

namespace emitter::ir {

    class expr_stmt_ir : public stmt_ir {
    public:
        expr_stmt_ir(std::unique_ptr<expr_ir> expr) :
                expr(std::move(expr)) {}

        std::unique_ptr<expr_ir> expr;
    };

}

#endif //SERALANG_EXPR_STMT_IR_H
