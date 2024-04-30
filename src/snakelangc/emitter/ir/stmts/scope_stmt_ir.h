#ifndef SNAKELANG_SCOPE_STMT_IR_H
#define SNAKELANG_SCOPE_STMT_IR_H

#include <vector>
#include <memory>
#include "stmt_ir.h"

namespace emitter::ir {

    class scope_stmt_ir : public stmt_ir {
    public:
        explicit scope_stmt_ir(scope_stmt_ir* parent_scope) : parent_scope(parent_scope) {}

        std::vector<std::unique_ptr<stmt_ir>> inner_stmts;

        scope_stmt_ir *parent_scope;

        std::map<std::string, type *> variables_types;
        std::map<std::string, type*> static_variables_types;

        std::vector<ir::variable_ir> static_variables;

        type* get_type_for_variable(const std::string& name) {
            if (variables_types.contains(name)) {
                return variables_types[name];
            }

            if (parent_scope == nullptr) {
                return nullptr;
            }

            return parent_scope->get_type_for_variable(name);
        }

        [[nodiscard]] bool is_var_exists(const std::string& name) const {
            if (variables_types.contains(name)) {
                return true;
            }

            if (parent_scope == nullptr) {
                return false;
            }

            return parent_scope->is_var_exists(name);
        }
    };

}

#endif //SNAKELANG_SCOPE_STMT_IR_H
