#ifndef SERALANG_POSTFIX_EXPR_IR_H
#define SERALANG_POSTFIX_EXPR_IR_H

#include "unary_expr_ir.h"

namespace emitter::ir {

    class postfix_expr_ir : public unary_expr_ir {
    public:
        postfix_expr_ir(std::unique_ptr<expr_ir> expr, unary_operation_type operation) :
                unary_expr_ir(std::move(expr), operation) {}
    };

}

#endif //SERALANG_POSTFIX_EXPR_IR_H
