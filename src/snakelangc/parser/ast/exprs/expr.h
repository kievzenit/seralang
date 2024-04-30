#ifndef SNAKELANG_EXPR_H
#define SNAKELANG_EXPR_H

#include <utility>

#include "../ast_node.h"

namespace parser::ast {

    class expr : public ast_node {
    public:
        expr(bool is_const) : is_const(is_const) {}
        virtual ~expr() = default;

        bool is_const;
    };

}

#endif //SNAKELANG_EXPR_H
