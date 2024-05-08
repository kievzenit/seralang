#ifndef SERALANG_CALL_EXPR_IR_H
#define SERALANG_CALL_EXPR_IR_H

#include "expr_ir.h"

namespace emitter::ir {

    class call_expr_ir : public expr_ir {
    public:
        call_expr_ir(std::string function_name, std::vector<std::unique_ptr<expr_ir>> arguments, type* return_type) :
            expr_ir(return_type, false),
            function_name(std::move(function_name)),
            arguments(std::move(arguments)) {}

        std::string function_name;
        std::vector<std::unique_ptr<expr_ir>> arguments;
    };

}

#endif //SERALANG_CALL_EXPR_IR_H
