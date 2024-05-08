#ifndef SERALANG_UPCAST_EXPR_IR_H
#define SERALANG_UPCAST_EXPR_IR_H

#include "cast_expr_ir.h"

namespace emitter::ir {

    class upcast_expr_ir : public cast_expr_ir {
    public:
        upcast_expr_ir(type* upcast_to_type, std::unique_ptr<expr_ir> inner_expr) :
            cast_expr_ir(upcast_to_type, std::move(inner_expr)) {}
    };

}

#endif //SERALANG_UPCAST_EXPR_IR_H
