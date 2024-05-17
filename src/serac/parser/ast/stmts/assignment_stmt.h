#ifndef SERALANG_ASSIGNMENT_STMT_H
#define SERALANG_ASSIGNMENT_STMT_H

#include <memory>
#include "local_stmt.h"
#include "../exprs/assignment_expr.h"

namespace parser::ast {

    class assignment_stmt : public local_stmt {
    public:
        explicit assignment_stmt(std::unique_ptr<ast::assignment_expr> assignment_expr) :
            assignment_expr(std::move(assignment_expr)) {}

        std::unique_ptr<ast::assignment_expr> assignment_expr;
    };

}

#endif //SERALANG_ASSIGNMENT_STMT_H
