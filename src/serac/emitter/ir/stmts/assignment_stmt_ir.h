#ifndef SERALANG_ASSIGNMENT_STMT_IR_H
#define SERALANG_ASSIGNMENT_STMT_IR_H

#include <string>
#include <memory>
#include <utility>
#include "stmt_ir.h"
#include "../exprs/assignment_expr_ir.h"

namespace emitter::ir {

    class assignment_stmt_ir : public stmt_ir {
    public:
        explicit assignment_stmt_ir(std::unique_ptr<assignment_expr_ir> assignment_expr) :
            assignment_expr(std::move(assignment_expr)) {}

        std::unique_ptr<assignment_expr_ir> assignment_expr;
    };

}

#endif //SERALANG_ASSIGNMENT_STMT_IR_H
