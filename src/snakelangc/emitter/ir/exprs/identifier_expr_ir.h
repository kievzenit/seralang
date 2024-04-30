#ifndef SNAKELANG_IDENTIFIER_EXPR_IR_H
#define SNAKELANG_IDENTIFIER_EXPR_IR_H

#include "expr_ir.h"

#include <utility>

namespace emitter::ir {

    class identifier_expr_ir : public expr_ir {
    public:
        identifier_expr_ir(std::string name, bool is_global, type* identifier_type) :
            expr_ir(identifier_type, false), is_global(is_global), name(std::move(name)) {}

        std::string name;
        bool is_global;
    };

}

#endif //SNAKELANG_IDENTIFIER_EXPR_IR_H
