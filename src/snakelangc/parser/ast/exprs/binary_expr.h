#ifndef SNAKELANG_BINARY_EXPR_H
#define SNAKELANG_BINARY_EXPR_H

#include <memory>
#include "expr.h"
#include "../common/binary_operation.h"

namespace parser::ast {

    class binary_expr : public expr {
    public:
        binary_expr(std::unique_ptr<expr> left, std::unique_ptr<expr> right, binary_operation operation) :
            expr(false),
            left(std::move(left)), right(std::move(right)),
            operation(operation) {}

        binary_operation operation;
        std::unique_ptr<expr> left, right;
    };

}

#endif //SNAKELANG_BINARY_EXPR_H
