#ifndef SNAKELANG_PACKAGE_IR_H
#define SNAKELANG_PACKAGE_IR_H

#include <utility>
#include <vector>
#include "variable_ir.h"

namespace emitter::ir {

    class package_ir {
    public:
        package_ir(std::string name, std::vector<std::unique_ptr<variable_ir>> global_variables) :
            name(std::move(name)), global_variables(std::move(global_variables)) {}

        std::string name;
        std::vector<std::unique_ptr<variable_ir>> global_variables;
    };

}

#endif //SNAKELANG_PACKAGE_IR_H
