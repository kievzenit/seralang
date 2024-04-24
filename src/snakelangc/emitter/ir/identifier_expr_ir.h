#ifndef SNAKELANG_IDENTIFIER_EXPR_IR_H
#define SNAKELANG_IDENTIFIER_EXPR_IR_H

#include "expr_ir.h"

#include <utility>

namespace emitter::ir {

    class identifier_expr_ir : public expr_ir {
    public:
        identifier_expr_ir(std::string name, type* identifier_type) :
            expr_ir(identifier_type, false), name(std::move(name)) {}

        std::string name;
    };

}

#endif //SNAKELANG_IDENTIFIER_EXPR_IR_H
