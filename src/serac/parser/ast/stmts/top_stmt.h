#ifndef SERALANG_TOP_STMT_H
#define SERALANG_TOP_STMT_H

#include "../ast_node.h"

namespace parser::ast {

    class top_stmt : public ast_node {
    public:
        virtual ~top_stmt() = default;
    };

}

#endif //SERALANG_TOP_STMT_H
