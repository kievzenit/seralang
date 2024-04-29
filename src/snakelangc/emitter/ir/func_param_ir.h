#ifndef SNAKELANG_FUNC_PARAM_IR_H
#define SNAKELANG_FUNC_PARAM_IR_H

#include <string>
#include "type.h"

namespace emitter::ir {

    class func_param_ir {
    public:
        func_param_ir(std::string name, type* param_type) : name(std::move(name)), param_type(param_type) {}

        std::string name;
        type* param_type;
    };

}

#endif //SNAKELANG_FUNC_PARAM_IR_H
