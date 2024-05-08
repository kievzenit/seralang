#ifndef SERALANG_ELSE_STMT_IR_H
#define SERALANG_ELSE_STMT_IR_H

#include <memory>
#include "stmt_ir.h"
#include "scope_stmt_ir.h"

namespace emitter::ir {

    class else_stmt_ir : public stmt_ir {
    public:
        explicit else_stmt_ir(std::unique_ptr<scope_stmt_ir> scope) : scope(std::move(scope)) {}

        std::unique_ptr<scope_stmt_ir> scope;
    };

}

#endif //SERALANG_ELSE_STMT_IR_H
