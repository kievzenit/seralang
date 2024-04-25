#ifndef SNAKELANG_CAST_EXPR_IR_H
#define SNAKELANG_CAST_EXPR_IR_H

#include "expr_ir.h"

namespace emitter::ir {

    class cast_expr_ir : public expr_ir {
    public:
        cast_expr_ir(type* cast_to_type, std::unique_ptr<expr_ir> inner_expr) :
                expr_ir(cast_to_type, false), inner_expr(std::move(inner_expr)) {}

        std::unique_ptr<expr_ir> inner_expr;
    };

}

#endif //SNAKELANG_CAST_EXPR_IR_H
