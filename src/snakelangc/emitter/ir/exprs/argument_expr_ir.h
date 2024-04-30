#ifndef SNAKELANG_ARGUMENT_EXPR_IR_H
#define SNAKELANG_ARGUMENT_EXPR_IR_H

#include "identifier_expr_ir.h"

namespace emitter::ir {

    class argument_exp_ir : public identifier_expr_ir {
    public:
        argument_exp_ir(std::string name, type* argument_type, int position) :
                identifier_expr_ir(std::move(name), false, argument_type),
                position(position) {}

        int position;
    };

}

#endif //SNAKELANG_ARGUMENT_EXPR_IR_H
