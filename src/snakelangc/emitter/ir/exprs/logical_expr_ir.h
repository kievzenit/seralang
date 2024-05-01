#ifndef SNAKELANG_LOGICAL_EXPR_IR_H
#define SNAKELANG_LOGICAL_EXPR_IR_H

#include "binary_expr_ir.h"

namespace emitter::ir {

    class logical_expr_ir : public binary_expr_ir {
    public:
        logical_expr_ir(
                std::unique_ptr<ir::expr_ir> left,
                std::unique_ptr<ir::expr_ir> right,
                binary_operation_type operation_type) :
            binary_expr_ir(std::move(left), std::move(right), operation_type, type::boolean()) {}
    };

}

#endif //SNAKELANG_LOGICAL_EXPR_IR_H
