#ifndef SNAKELANG_TYPE_H
#define SNAKELANG_TYPE_H

#include <string>

namespace emitter::ir {

    class type {
    public:
        type(std::string name, int size, bool is_basic = false) :
            name(std::move(name)), size(size), is_basic(is_basic) {}

        bool is_basic;
        std::string name;
        int size;

        [[nodiscard]] virtual bool can_be_implicitly_casted_to(type* other) const;

        static type* boolean();

    private:
        static type* boolean_;
    };

}

#endif //SNAKELANG_TYPE_H
