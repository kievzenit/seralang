#ifndef SNAKELANG_BOOLEAN_EXPR_IR_H
#define SNAKELANG_BOOLEAN_EXPR_IR_H

#include "expr_ir.h"
#include "../../parser/ast/boolean_expr.h"

namespace emitter::ir {

    class boolean_expr_ir : public expr_ir {
    public:
        boolean_expr_ir(parser::ast::boolean_expr *expr) : expr_ir(type::boolean()), value(expr->value) {}

        bool value;
    };

}

#endif //SNAKELANG_BOOLEAN_EXPR_IR_H
