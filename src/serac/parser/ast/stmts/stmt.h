#ifndef SERALANG_STMT_H
#define SERALANG_STMT_H

#include "../ast_node.h"

namespace parser::ast {

    class stmt: public ast_node {
    public:
        virtual ~stmt() = default;
    };

}

#endif //SERALANG_STMT_H
