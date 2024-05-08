#ifndef SERALANG_BINARY_EXPR_IR_H
#define SERALANG_BINARY_EXPR_IR_H

#include "expr_ir.h"
#include "../../../parser/ast/exprs/binary_expr.h"
#include "../types/binary_operation_type.h"

namespace emitter::ir {

    class binary_expr_ir : public expr_ir {
    public:
        binary_expr_ir(
                std::unique_ptr<expr_ir> left,
                std::unique_ptr<expr_ir> right,
                binary_operation_type operation_type,
                type* type) :
            left(std::move(left)), right(std::move(right)),
            operation_type(operation_type),
            expr_ir(type, false) {}

        std::unique_ptr<expr_ir> left, right;
        binary_operation_type operation_type;
    };

}

#endif //SERALANG_BINARY_EXPR_IR_H
