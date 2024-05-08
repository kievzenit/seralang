#ifndef SERALANG_BOOLEAN_EXPR_H
#define SERALANG_BOOLEAN_EXPR_H

#include <format>
#include "expr.h"

namespace parser::ast {

    class boolean_expr : public expr {
    public:
        boolean_expr(bool value) : expr(true), value(value) {}

        bool value;
    };

}

#endif //SERALANG_BOOLEAN_EXPR_H
