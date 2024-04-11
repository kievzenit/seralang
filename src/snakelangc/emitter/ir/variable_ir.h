#ifndef SNAKELANG_VARIABLE_IR_H
#define SNAKELANG_VARIABLE_IR_H

#include <string>
#include <utility>
#include "basic_type.h"
#include "expr_ir.h"

namespace emitter::ir {

    class variable_ir {
    public:
        variable_ir(std::string name, std::unique_ptr<expr_ir> expr, basic_type type) :
            name(std::move(name)), expr(std::move(expr)), type(std::move(type)) {}

        basic_type type;
        std::string name;
        std::unique_ptr<expr_ir> expr;
    };

}

#endif //SNAKELANG_VARIABLE_IR_H
