#ifndef SNAKELANG_FUNC_DECL_STMT_H
#define SNAKELANG_FUNC_DECL_STMT_H

#include <string>
#include <memory>
#include <utility>
#include "top_stmt.h"
#include "scope_stmt.h"
#include "../common/func_param.h"

namespace parser::ast {

    class func_decl_stmt : public top_stmt {
    public:
        func_decl_stmt(
                std::string name,
                std::vector<func_param> params,
                std::string return_type,
                std::unique_ptr<scope_stmt> func_scope) :
                name(std::move(name)),
                params(std::move(params)),
                return_type(std::move(return_type)),
                func_scope(std::move(func_scope)) {}

        std::string name;
        std::vector<func_param> params;
        std::string return_type;
        std::unique_ptr<scope_stmt> func_scope;
    };

}

#endif //SNAKELANG_FUNC_DECL_STMT_H
