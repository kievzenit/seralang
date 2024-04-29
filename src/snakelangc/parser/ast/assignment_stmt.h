#ifndef SNAKELANG_ASSIGNMENT_STMT_H
#define SNAKELANG_ASSIGNMENT_STMT_H

#include <memory>
#include "stmt.h"
#include "assignment_expr.h"

namespace parser::ast {

    class assignment_stmt : public stmt {
    public:
        explicit assignment_stmt(std::string name, std::unique_ptr<ast::expr> assignment_expr) :
            name(std::move(name)), assignment_expr(std::move(assignment_expr)) {}

        std::string name;
        std::unique_ptr<ast::expr> assignment_expr;
    };

}

#endif //SNAKELANG_ASSIGNMENT_STMT_H
