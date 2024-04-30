#ifndef SNAKELANG_FUNC_PARAM_H
#define SNAKELANG_FUNC_PARAM_H

#include <string>
#include <utility>

namespace parser::ast {

    class func_param {
    public:
        func_param(std::string name, std::string type) : name(std::move(name)), type(std::move(type)) {}

        std::string name;
        std::string type;
    };

}

#endif //SNAKELANG_FUNC_PARAM_H
