#ifndef SNAKELANG_IDENTIFIER_EXPR_H
#define SNAKELANG_IDENTIFIER_EXPR_H

#include "expr.h"

#include <utility>
#include <format>

namespace parser::ast {

    class identifier_expr : public expr {
    public:
        identifier_expr(std::string name) : name(std::move(name)) {}

        std::string name;
    };

}

#endif //SNAKELANG_IDENTIFIER_EXPR_H
