#ifndef SERALANG_SCOPE_H
#define SERALANG_SCOPE_H

#include <map>
#include <string>
#include <llvm/IR/Value.h>

namespace translator {

    class scope {
    public:
        explicit scope(scope* parent_scope) : parent_scope(parent_scope) {}

        scope* parent_scope;
        std::map<std::string, llvm::Value*> local_variables;

        llvm::Value* get_variable(const std::string &variable_name) {
            auto variable_value = local_variables[variable_name];

            return variable_value ? variable_value : parent_scope->get_variable(variable_name);
        }
    };

}

#endif //SERALANG_SCOPE_H
