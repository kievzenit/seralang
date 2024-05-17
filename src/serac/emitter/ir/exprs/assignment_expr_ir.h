#ifndef SERALANG_ASSIGNMENT_EXPR_IR_H
#define SERALANG_ASSIGNMENT_EXPR_IR_H

#include "expr_ir.h"

namespace emitter::ir {

    class assignment_expr_ir : public expr_ir {
    public:
        assignment_expr_ir(std::string identifier_name, bool is_global, std::unique_ptr<expr_ir> inner_expr) :
                expr_ir(inner_expr->expr_type, inner_expr->is_const_expr),
                identifier_name(std::move(identifier_name)),
                is_global(is_global),
                inner_expr(std::move(inner_expr)) {}

        std::string identifier_name;
        bool is_global;
        std::unique_ptr<expr_ir> inner_expr;
    };

}

#endif //SERALANG_ASSIGNMENT_EXPR_IR_H
