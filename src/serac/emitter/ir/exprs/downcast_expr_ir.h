#ifndef SERALANG_DOWNCAST_EXPR_IR_H
#define SERALANG_DOWNCAST_EXPR_IR_H

#include "cast_expr_ir.h"

namespace emitter::ir {

    class downcast_expr_ir : public cast_expr_ir {
    public:
        downcast_expr_ir(type* downcast_to_type, std::unique_ptr<expr_ir> inner_expr) :
            cast_expr_ir(downcast_to_type, std::move(inner_expr)) {}
    };

}

#endif //SERALANG_DOWNCAST_EXPR_IR_H
