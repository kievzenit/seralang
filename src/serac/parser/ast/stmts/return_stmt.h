#ifndef SERALANG_RETURN_STMT_H
#define SERALANG_RETURN_STMT_H

#include <memory>
#include "control_flow_stmt.h"
#include "../exprs/expr.h"

namespace parser::ast {

    class return_stmt : public control_flow_stmt {
    public:
        return_stmt(std::unique_ptr<expr> return_expr) : return_expr(std::move(return_expr)) {}

        std::unique_ptr<expr> return_expr;
    };

}

#endif //SERALANG_RETURN_STMT_H
