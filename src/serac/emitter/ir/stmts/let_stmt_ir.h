#ifndef SERALANG_LET_STMT_IR_H
#define SERALANG_LET_STMT_IR_H

#include <string>
#include <utility>
#include "emitter/ir/types/type.h"
#include "emitter/ir/exprs/expr_ir.h"
#include "stmt_ir.h"

namespace emitter::ir {

    class let_stmt_ir : public stmt_ir {
    public:
        let_stmt_ir(std::string name, std::unique_ptr<expr_ir> expr, type* type, bool is_static, bool is_private) :
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

#endif //SERALANG_LET_STMT_IR_H
