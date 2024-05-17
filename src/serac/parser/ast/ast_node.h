#ifndef SERALANG_AST_NODE_H
#define SERALANG_AST_NODE_H

#include <utility>

#include "common/file_metadata.h"

namespace parser::ast {

    class ast_node {
    public:
        virtual ~ast_node() = default;

        file_metadata metadata;
    };

}

#endif //SERALANG_AST_NODE_H
