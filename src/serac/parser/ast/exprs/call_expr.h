#ifndef SERALANG_CALL_EXPR_H
#define SERALANG_CALL_EXPR_H

#include <string>
#include <utility>

#include "expr.h"

namespace parser::ast {

    class call_expr : public expr {
    public:
        call_expr(std::string name, std::vector<std::unique_ptr<expr>> arguments) :
            expr(false), name(std::move(name)), arguments(std::move(arguments)) {}

    public:
        std::string name;
        std::vector<std::unique_ptr<expr>> arguments;
    };

}

#endif //SERALANG_CALL_EXPR_H
