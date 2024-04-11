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
        expr_ir(type type) : expr_type(std::move(type)) {}
        virtual ~expr_ir() = default;

        type expr_type;
    };

}

#endif //SNAKELANG_EXPR_IR_H
