#ifndef SNAKELANG_ASSIGNMENT_STMT_IR_H
#define SNAKELANG_ASSIGNMENT_STMT_IR_H

#include <string>
#include <memory>
#include <utility>
#include "stmt_ir.h"
#include "expr_ir.h"

namespace emitter::ir {

    class assignment_stmt_ir : public stmt_ir {
    public:
        assignment_stmt_ir(std::string identifier_name, bool is_global, std::unique_ptr<expr_ir> assignment_expr) :
            identifier_name(std::move(identifier_name)),
            is_global(is_global),
            assignment_expr(std::move(assignment_expr)) {}

        std::string identifier_name;
        bool is_global;
        std::unique_ptr<expr_ir> assignment_expr;
    };

}

#endif //SNAKELANG_ASSIGNMENT_STMT_IR_H
