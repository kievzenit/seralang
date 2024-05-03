#ifndef SNAKELANG_WHILLE_STMT_IR_H
#define SNAKELANG_WHILLE_STMT_IR_H

#include <memory>
#include "stmt_ir.h"
#include "../exprs/expr_ir.h"
#include "scope_stmt_ir.h"

namespace emitter::ir {

    class while_stmt_ir : public stmt_ir {
    public:
        while_stmt_ir(std::unique_ptr<expr_ir> condition, std::unique_ptr<scope_stmt_ir> scope) :
            condition(std::move(condition)), scope(std::move(scope)) {}

        std::unique_ptr<expr_ir> condition;
        std::unique_ptr<scope_stmt_ir> scope;
    };

}

#endif //SNAKELANG_WHILLE_STMT_IR_H
