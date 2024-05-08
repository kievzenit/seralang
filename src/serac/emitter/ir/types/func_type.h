#ifndef SERALANG_FUNC_TYPE_H
#define SERALANG_FUNC_TYPE_H

#include <string>
#include <vector>
#include "type.h"
#include "func_param_ir.h"

namespace emitter::ir {

    class func_type {
    public:
        func_type(std::string name, std::vector<ir::func_param_ir> params, ir::type* return_type) :
            name(std::move(name)),
            params(std::move(params)),
            return_type(return_type) {}

        std::string name;
        std::vector<ir::func_param_ir> params;
        ir::type* return_type;
    };

}

#endif //SERALANG_FUNC_TYPE_H
