#ifndef SERALANG_LOOP_STMT_IR_H
#define SERALANG_LOOP_STMT_IR_H

#include <memory>
#include "stmt_ir.h"
#include "scope_stmt_ir.h"

namespace emitter::ir {

    class loop_stmt_ir : public stmt_ir {
    public:
        explicit loop_stmt_ir(std::unique_ptr<scope_stmt_ir> scope) : scope(std::move(scope)) {}

        std::unique_ptr<scope_stmt_ir> scope;
    };

}

#endif //SERALANG_LOOP_STMT_IR_H
