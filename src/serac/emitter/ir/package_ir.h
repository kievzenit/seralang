#ifndef SERALANG_PACKAGE_IR_H
#define SERALANG_PACKAGE_IR_H

#include <utility>
#include <vector>
#include "stmts/let_stmt_ir.h"
#include "func_decl_ir.h"

namespace emitter::ir {

    class package_ir {
    public:
        package_ir(
            std::string name,
            std::vector<std::unique_ptr<let_stmt_ir>> global_variables,
            std::vector<std::unique_ptr<func_decl_ir>> func_declarations) :
            name(std::move(name)),
            global_variables(std::move(global_variables)),
            func_declarations(std::move(func_declarations)) {}

        std::string name;
        std::vector<std::unique_ptr<let_stmt_ir>> global_variables;
        std::vector<std::unique_ptr<func_decl_ir>> func_declarations;
    };

}

#endif //SERALANG_PACKAGE_IR_H
