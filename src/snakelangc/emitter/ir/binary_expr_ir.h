#ifndef SNAKELANG_BINARY_EXPR_IR_H
#define SNAKELANG_BINARY_EXPR_IR_H

#include "expr_ir.h"
#include "../../parser/ast/binary_expr.h"

namespace emitter::ir {

    class binary_expr_ir : public expr_ir {
    public:
        binary_expr_ir(std::unique_ptr<expr_ir> left, std::unique_ptr<expr_ir> right, type type) :
            left(std::move(left)), right(std::move(right)),
            expr_ir(std::move(type)) {}

        std::unique_ptr<expr_ir> left, right;
    };

}

#endif //SNAKELANG_BINARY_EXPR_IR_H
