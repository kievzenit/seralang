#ifndef SERALANG_ASSIGNMENT_STMT_H
#define SERALANG_ASSIGNMENT_STMT_H

#include <memory>
#include "local_stmt.h"

namespace parser::ast {

    class assignment_stmt : public local_stmt {
    public:
        explicit assignment_stmt(std::string name, std::unique_ptr<ast::expr> assignment_expr) :
            name(std::move(name)), assignment_expr(std::move(assignment_expr)) {}

        std::string name;
        std::unique_ptr<ast::expr> assignment_expr;
    };

}

#endif //SERALANG_ASSIGNMENT_STMT_H
