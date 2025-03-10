#ifndef SERALANG_BOOLEAN_EXPR_IR_H
#define SERALANG_BOOLEAN_EXPR_IR_H

#include "expr_ir.h"
#include "../../../parser/ast/exprs/boolean_expr.h"

namespace emitter::ir {

    class boolean_expr_ir : public expr_ir {
    public:
        boolean_expr_ir(parser::ast::boolean_expr *expr) :
            expr_ir(type::boolean(), true), value(expr->value) {}

        bool value;
    };

}

#endif //SERALANG_BOOLEAN_EXPR_IR_H
