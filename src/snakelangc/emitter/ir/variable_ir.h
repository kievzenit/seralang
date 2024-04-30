#ifndef SNAKELANG_VARIABLE_IR_H
#define SNAKELANG_VARIABLE_IR_H

#include <string>
#include <utility>
#include "types/type.h"
#include "exprs/expr_ir.h"
#include "stmts/stmt_ir.h"

namespace emitter::ir {

    class variable_ir : public stmt_ir {
    public:
        variable_ir(std::string name, std::unique_ptr<expr_ir> expr, type* type, bool is_static, bool is_private) :
                name(std::move(name)),
                expr(std::move(expr)),
                variable_type(type),
                is_static(is_static),
                is_private(is_private) {}

        type* variable_type;
        std::string name;
        std::unique_ptr<expr_ir> expr;
        bool is_static;
        bool is_private;
    };

}

#endif //SNAKELANG_VARIABLE_IR_H
