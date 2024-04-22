#ifndef SNAKELANG_CALL_EXPR_H
#define SNAKELANG_CALL_EXPR_H

#include <string>
#include "expr.h"

namespace parser::ast {

    class call_expr : public expr {
    public:
        call_expr(std::string name) : name(std::move(name)) {}

    public:
        std::string name;
    };

}

#endif //SNAKELANG_CALL_EXPR_H
