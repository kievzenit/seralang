#ifndef SNAKELANG_BOOLEAN_EXPR_H
#define SNAKELANG_BOOLEAN_EXPR_H

#include <format>
#include "expr.h"

namespace parser::ast {

    class boolean_expr : public expr {
    public:
        boolean_expr(bool value) : value(value) {}

        bool value;
    };

}

#endif //SNAKELANG_BOOLEAN_EXPR_H
