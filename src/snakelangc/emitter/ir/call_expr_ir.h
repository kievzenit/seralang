#ifndef SNAKELANG_CALL_EXPR_IR_H
#define SNAKELANG_CALL_EXPR_IR_H

#include "expr_ir.h"

namespace emitter::ir {

    class call_expr_ir : public expr_ir {
    public:
        call_expr_ir(std::string function_name, type* return_type) :
            expr_ir(return_type, false),
            function_name(std::move(function_name)) {}

        std::string function_name;
    };

}

#endif //SNAKELANG_CALL_EXPR_IR_H
