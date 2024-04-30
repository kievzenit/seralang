#ifndef SNAKELANG_TYPE_H
#define SNAKELANG_TYPE_H

#include <string>

namespace emitter::ir {

    class type {
    public:
        type(std::string name, int size, bool is_basic = false) :
            name(std::move(name)), size(size), is_basic(is_basic), is_unsigned(false) {}

        bool is_basic;
        std::string name;
        int size;
        bool is_unsigned;

        [[nodiscard]] bool can_be_implicitly_casted_to(type* other) const;

        static type* boolean();
        static type* int1();
        static type* int8();
        static type* int16();
        static type* int32();
        static type* int64();
        static type* uint8();
        static type* uint16();
        static type* uint32();
        static type* uint64();

    private:
        type(std::string name, int size, bool is_basic, bool is_unsigned) :
            name(std::move(name)), size(size), is_basic(is_basic), is_unsigned(is_unsigned) {}

        static type* boolean_;
        static type* int1_;
        static type* int8_;
        static type* int16_;
        static type* int32_;
        static type* int64_;
        static type* uint8_;
        static type* uint16_;
        static type* uint32_;
        static type* uint64_;
    };

}

#endif //SNAKELANG_TYPE_H
