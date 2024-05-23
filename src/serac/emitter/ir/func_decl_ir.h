#ifndef SERALANG_FUNC_DECL_IR_H
#define SERALANG_FUNC_DECL_IR_H

#include <string>
#include <memory>
#include <utility>
#include "types/type.h"
#include "stmts/scope_stmt_ir.h"
#include "types/func_param_ir.h"

namespace emitter::ir {

    class func_decl_ir {
    public:
        func_decl_ir(
                std::string name,
                std::vector<func_param_ir> params,
                type* return_type,
                bool is_extern,
                std::unique_ptr<scope_stmt_ir> scope) :
                name(std::move(name)),
                params(std::move(params)),
                return_type(return_type),
                is_extern(is_extern),
                scope(std::move(scope)) {}

        std::string name;
        std::vector<func_param_ir> params;
        type* return_type;
        bool is_extern;
        std::unique_ptr<scope_stmt_ir> scope;
    };

}

#endif //SERALANG_FUNC_DECL_IR_H
