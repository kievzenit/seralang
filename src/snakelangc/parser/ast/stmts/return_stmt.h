#ifndef SNAKELANG_RETURN_STMT_H
#define SNAKELANG_RETURN_STMT_H

#include <memory>
#include "stmt.h"
#include "../exprs/expr.h"

namespace parser::ast {

    class return_stmt : public stmt {
    public:
        return_stmt(std::unique_ptr<expr> return_expr) : return_expr(std::move(return_expr)) {}

        std::unique_ptr<expr> return_expr;
    };

}

#endif //SNAKELANG_RETURN_STMT_H
