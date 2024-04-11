#ifndef SNAKELANG_EXPR_H
#define SNAKELANG_EXPR_H

#include <utility>

#include "ast_node.h"

namespace parser::ast {

    class expr : public ast_node {
    public:
        virtual ~expr() = default;
    };

}

#endif //SNAKELANG_EXPR_H
