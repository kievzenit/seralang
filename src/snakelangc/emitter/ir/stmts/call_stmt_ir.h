#ifndef SNAKELANG_CALL_STMT_IR_H
#define SNAKELANG_CALL_STMT_IR_H

#include <memory>
#include "stmt_ir.h"
#include "../exprs/call_expr_ir.h"

namespace emitter::ir {

    class call_stmt_ir : public stmt_ir {
    public:
        call_stmt_ir(std::unique_ptr<call_expr_ir> call_expr_ir) : call_expr(std::move(call_expr_ir)) {}

        std::unique_ptr<call_expr_ir> call_expr;
    };

}

#endif //SNAKELANG_CALL_STMT_IR_H
