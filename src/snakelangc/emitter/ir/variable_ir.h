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
        variable_ir(std::string name, std::unique_ptr<expr_ir> expr, type* type, bool is_static) :
                name(std::move(name)), expr(std::move(expr)), variable_type(type), is_static(is_static) {}

        type* variable_type;
        std::string name;
        std::unique_ptr<expr_ir> expr;
        bool is_static;
    };

}

#endif //SNAKELANG_VARIABLE_IR_H
