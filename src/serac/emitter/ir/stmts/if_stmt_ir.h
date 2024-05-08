#ifndef SERALANG_IF_STMT_IR_H
#define SERALANG_IF_STMT_IR_H

#include <memory>
#include "stmt_ir.h"
#include "../exprs/expr_ir.h"
#include "scope_stmt_ir.h"
#include "else_if_stmt_ir.h"
#include "else_stmt_ir.h"

namespace emitter::ir {

    class if_stmt_ir : public stmt_ir {
    public:
        if_stmt_ir(
                std::unique_ptr<expr_ir> if_expr,
                std::unique_ptr<scope_stmt_ir> scope,
                std::vector<std::unique_ptr<else_if_stmt_ir>> else_if_branches,
                std::unique_ptr<else_stmt_ir> else_branch) :
                if_expr(std::move(if_expr)),
                scope(std::move(scope)),
                else_if_branches(std::move(else_if_branches)),
                else_branch(std::move(else_branch)) {}

        std::unique_ptr<expr_ir> if_expr;
        std::unique_ptr<scope_stmt_ir> scope;

        std::vector<std::unique_ptr<else_if_stmt_ir>> else_if_branches;
        std::unique_ptr<else_stmt_ir> else_branch;
    };

}

#endif //SERALANG_IF_STMT_IR_H
