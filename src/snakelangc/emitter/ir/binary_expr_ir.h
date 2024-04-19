#ifndef SNAKELANG_BINARY_EXPR_IR_H
#define SNAKELANG_BINARY_EXPR_IR_H

#include "expr_ir.h"
#include "../../parser/ast/binary_expr.h"
#include "binary_operation_type.h"

namespace emitter::ir {

    class binary_expr_ir : public expr_ir {
    public:
        binary_expr_ir(
                std::unique_ptr<expr_ir> left,
                std::unique_ptr<expr_ir> right,
                binary_operation_type operation_type,
                type type) :
            left(std::move(left)), right(std::move(right)),
            operation_type(operation_type),
            expr_ir(std::move(type), false) {}

        binary_expr_ir(
                std::unique_ptr<expr_ir> left,
                std::unique_ptr<expr_ir> right,
                binary_operation_type operation_type,
                bool left_should_be_upcasted,
                bool right_should_be_upcasted,
                type type) :
                left(std::move(left)), right(std::move(right)),
                left_should_be_upcasted(left_should_be_upcasted),
                right_should_be_upcasted(right_should_be_upcasted),
                operation_type(operation_type),
                expr_ir(std::move(type), false) {}

        std::unique_ptr<expr_ir> left, right;
        bool left_should_be_upcasted, right_should_be_upcasted;
        binary_operation_type operation_type;
    };

}

#endif //SNAKELANG_BINARY_EXPR_IR_H
