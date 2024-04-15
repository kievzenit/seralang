#ifndef SNAKELANG_INTEGER_EXPR_IR_H
#define SNAKELANG_INTEGER_EXPR_IR_H

#include "../../parser/ast/integer_expr.h"
#include "type.h"
#include "expr_ir.h"

#include <utility>

namespace emitter::ir {

    class integer_expr_ir : public expr_ir {
    public:
        integer_expr_ir(parser::ast::integer_expr *integer_expr) :
            expr_ir(type::int32(), true), number(integer_expr->number) {}

        integer_expr_ir(parser::ast::integer_expr *integer_expr, type integer_type) :
            expr_ir(std::move(integer_type), true), number(integer_expr->number) {}

        int number;
    };

}

#endif //SNAKELANG_INTEGER_EXPR_IR_H
