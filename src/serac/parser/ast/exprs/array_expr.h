#ifndef SERALANG_ARRAY_EXPRESSION_H
#define SERALANG_ARRAY_EXPRESSION_H

#include <vector>
#include <memory>

#include "expr.h"

namespace parser::ast {

    class array_expr : public expr {
    public:
        array_expr(int length, std::vector<std::unique_ptr<expr>> initialization_list) :
                expr(false),
                length(length),
                initialization_list(std::move(initialization_list)) {}

        int length;
        std::vector<std::unique_ptr<expr>> initialization_list;
    };

}

#endif //SERALANG_ARRAY_EXPRESSION_H
