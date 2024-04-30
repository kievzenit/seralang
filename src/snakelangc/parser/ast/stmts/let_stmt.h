#ifndef SNAKELANG_LET_STMT_H
#define SNAKELANG_LET_STMT_H

#include <string>
#include "../exprs/expr.h"
#include "stmt.h"

namespace parser::ast {

    class let_stmt : public top_stmt, public stmt {
    public:
        let_stmt(std::string name, std::unique_ptr<expr> expression, bool is_static) :
            name(std::move(name)), expression(std::move(expression)), is_static(is_static) {}

        std::string name;
        std::unique_ptr<expr> expression;
        bool is_static;
    };

}

#endif //SNAKELANG_LET_STMT_H
