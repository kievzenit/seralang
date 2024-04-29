#ifndef SNAKELANG_FUNC_DECL_IR_H
#define SNAKELANG_FUNC_DECL_IR_H

#include <string>
#include <memory>
#include <utility>
#include "type.h"
#include "scope_stmt_ir.h"
#include "func_param_ir.h"

namespace emitter::ir {

    class func_decl_ir {
    public:
        func_decl_ir(
                std::string name,
                std::vector<func_param_ir> params,
                type* return_type,
                std::unique_ptr<scope_stmt_ir> root_scope_stmt) :
            name(std::move(name)),
            params(std::move(params)),
            return_type(return_type),
            root_scope_stmt(std::move(root_scope_stmt)) {}

        std::string name;
        std::vector<func_param_ir> params;
        type* return_type;
        std::unique_ptr<scope_stmt_ir> root_scope_stmt;
    };

}

#endif //SNAKELANG_FUNC_DECL_IR_H
