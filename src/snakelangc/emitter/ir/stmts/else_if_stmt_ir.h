#ifndef SNAKELANG_ELSE_IF_STMT_IR_H
#define SNAKELANG_ELSE_IF_STMT_IR_H

#include <memory>
#include "stmt_ir.h"
#include "../exprs/expr_ir.h"
#include "scope_stmt_ir.h"

namespace emitter::ir {

    class else_if_stmt_ir : public stmt_ir {
    public:
        else_if_stmt_ir(std::unique_ptr<expr_ir> if_expr, std::unique_ptr<scope_stmt_ir> scope) :
            if_expr(std::move(if_expr)), scope(std::move(scope)) {}

        std::unique_ptr<expr_ir> if_expr;
        std::unique_ptr<scope_stmt_ir> scope;
    };

}

#endif //SNAKELANG_ELSE_IF_STMT_IR_H
