#ifndef SERALANG_LET_STMT_H
#define SERALANG_LET_STMT_H

#include <string>
#include "../exprs/expr.h"
#include "local_stmt.h"

namespace parser::ast {

    class let_stmt : public top_stmt, public local_stmt {
    public:
        let_stmt(std::string name, std::unique_ptr<expr> expression, bool is_static) :
            name(std::move(name)), expression(std::move(expression)), is_static(is_static) {}

        std::string name;
        std::unique_ptr<expr> expression;
        bool is_static;
    };

}

#endif //SERALANG_LET_STMT_H
