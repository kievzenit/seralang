#ifndef SNAKELANG_VARIABLE_IR_H
#define SNAKELANG_VARIABLE_IR_H

#include <string>
#include <utility>
#include "type.h"
#include "expr_ir.h"
#include "stmt_ir.h"

namespace emitter::ir {

    class variable_ir : public stmt_ir {
    public:
        variable_ir(std::string name, std::unique_ptr<expr_ir> expr, type* type) :
                name(std::move(name)), expr(std::move(expr)), variable_type(type) {}

        type* variable_type;
        std::string name;
        std::unique_ptr<expr_ir> expr;
    };

}

#endif //SNAKELANG_VARIABLE_IR_H
