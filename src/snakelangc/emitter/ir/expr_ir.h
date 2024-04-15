#ifndef SNAKELANG_EXPR_IR_H
#define SNAKELANG_EXPR_IR_H

#include <memory>
#include <utility>
#include "../../parser/ast/expr.h"
#include "../../parser/ast/integer_expr.h"
#include "type.h"

namespace emitter::ir {

    class expr_ir {
    public:
        expr_ir(type type, bool is_const_expr) : expr_type(std::move(type)), is_const_expr(is_const_expr) {}
        virtual ~expr_ir() = default;

        type expr_type;
        bool is_const_expr;
    };

}

#endif //SNAKELANG_EXPR_IR_H
