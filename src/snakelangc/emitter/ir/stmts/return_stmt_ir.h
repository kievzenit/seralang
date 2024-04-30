#ifndef SNAKELANG_RETURN_STMT_IR_H
#define SNAKELANG_RETURN_STMT_IR_H

#include "../exprs/expr_ir.h"
#include "stmt_ir.h"

namespace emitter::ir {

    class return_stmt_ir : public stmt_ir {
    public:
        return_stmt_ir(std::unique_ptr<expr_ir> expr_ir) : expr(std::move(expr_ir)) {}

        std::unique_ptr<expr_ir> expr;
    };

}

#endif //SNAKELANG_RETURN_STMT_IR_H
