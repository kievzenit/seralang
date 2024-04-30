#ifndef SNAKELANG_EXPR_IR_H
#define SNAKELANG_EXPR_IR_H

#include <memory>
#include <utility>
#include "../../../parser/ast/exprs/expr.h"
#include "../../../parser/ast/exprs/integer_expr.h"
#include "../types/type.h"

namespace emitter::ir {

    class expr_ir {
    public:
        expr_ir(type* type, bool is_const_expr) : expr_type(type), is_const_expr(is_const_expr) {}
        virtual ~expr_ir() = default;

        type* expr_type;
        bool is_const_expr;
    };

}

#endif //SNAKELANG_EXPR_IR_H
