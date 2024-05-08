#ifndef SERALANG_IDENTIFIER_EXPR_H
#define SERALANG_IDENTIFIER_EXPR_H

#include <string>
#include "expr.h"

namespace parser::ast {

    class identifier_expr : public expr {
    public:
        identifier_expr(std::string name) : expr(false), name(std::move(name)) {}

        std::string name;
    };

}

#endif //SERALANG_IDENTIFIER_EXPR_H
