#ifndef SERALANG_UNARY_EXPR_IR_H
#define SERALANG_UNARY_EXPR_IR_H

#include "expr_ir.h"
#include "../types/unary_operation_type.h"

namespace emitter::ir {

    class unary_expr_ir : public expr_ir {
    public:
        unary_expr_ir(std::unique_ptr<expr_ir> expr, unary_operation_type operation) :
                expr_ir(expr->expr_type, expr->is_const_expr),
                expr(std::move(expr)),
                operation(operation) {}

        std::unique_ptr<expr_ir> expr;
        unary_operation_type operation;
    };

}

#endif //SERALANG_UNARY_EXPR_IR_H
