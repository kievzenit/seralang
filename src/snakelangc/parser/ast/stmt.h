#ifndef SNAKELANG_STMT_H
#define SNAKELANG_STMT_H

#include "ast_node.h"

namespace parser::ast {

    class stmt: public ast_node {
    public:
        virtual ~stmt() = default;
    };

}

#endif //SNAKELANG_STMT_H
