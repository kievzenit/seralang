#ifndef SERALANG_SCOPE_STMT_H
#define SERALANG_SCOPE_STMT_H

#include <memory>
#include "stmt.h"

namespace parser::ast {

    class scope_stmt : public stmt {
    public:
        scope_stmt(std::vector<std::unique_ptr<stmt>> inner_stmts) : inner_stmts(std::move(inner_stmts)) {}

        std::vector<std::unique_ptr<stmt>> inner_stmts;
    };

}

#endif //SERALANG_SCOPE_STMT_H
