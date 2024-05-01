#ifndef SNAKELANG_BITWISE_EXPR_IR_H
#define SNAKELANG_BITWISE_EXPR_IR_H

#include "binary_expr_ir.h"

namespace emitter::ir {

    class bitwise_expr_ir : public binary_expr_ir {
    public:
        bitwise_expr_ir(
                std::unique_ptr<ir::expr_ir> left,
                std::unique_ptr<ir::expr_ir> right,
                binary_operation_type operation_type,
                type* type) :
            binary_expr_ir(std::move(left), std::move(right), operation_type, type) {}
    };

}

#endif //SNAKELANG_BITWISE_EXPR_IR_H
