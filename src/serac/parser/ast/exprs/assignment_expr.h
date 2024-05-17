#ifndef SERALANG_ASSIGNMENT_EXPR_H
#define SERALANG_ASSIGNMENT_EXPR_H

#include <string>
#include <memory>

#include "expr.h"

namespace parser::ast {

    class assignment_expr : public expr {
    public:
        explicit assignment_expr(std::string name, std::unique_ptr<ast::expr> inner_expr) :
                expr(false), name(std::move(name)), inner_expr(std::move(inner_expr)) {}

        std::string name;
        std::unique_ptr<ast::expr> inner_expr;
    };

}

#endif //SERALANG_ASSIGNMENT_EXPR_H
