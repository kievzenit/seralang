#ifndef SERALANG_DO_WHILE_STMT_IR_H
#define SERALANG_DO_WHILE_STMT_IR_H

#include <memory>
#include "stmt_ir.h"
#include "../exprs/expr_ir.h"
#include "scope_stmt_ir.h"

namespace emitter::ir {

    class do_while_stmt_ir : public stmt_ir {
    public:
        do_while_stmt_ir(std::unique_ptr<expr_ir> condition, std::unique_ptr<scope_stmt_ir> scope) :
                condition(std::move(condition)), scope(std::move(scope)) {}

        std::unique_ptr<expr_ir> condition;
        std::unique_ptr<scope_stmt_ir> scope;
    };

}

#endif //SERALANG_DO_WHILE_STMT_IR_H
