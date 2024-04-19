#ifndef SNAKELANG_TYPE_H
#define SNAKELANG_TYPE_H

#include <string>

namespace emitter::ir {

    class type {
    public:
        type(std::string name, int size, bool is_basic = false) : name(std::move(name)), size(size), is_basic(is_basic) {}

        bool is_basic;
        std::string name;
        int size;

        [[nodiscard]] bool can_be_explicitly_casted_to(const type& other) const {
            if (*this == int1()) {
                return other == int8()
                    || other == uint16()
                    || other == int16()
                    || other == uint16()
                    || other == int32()
                    || other == uint32()
                    || other == int64()
                    || other == uint64();
            }

            if (*this == int8()) {
                return other == int16()
                    || other == int32()
                    || other == int64();
            }

            if (*this == int16()) {
                return other == int32()
                    || other == int64();
            }

            if (*this == int32()) {
                return other == int64();
            }

            return false;
        }

        bool operator==(const type &other) const {
            return is_basic == other.is_basic && size == other.size && name == other.name;
        }

        bool operator!=(const type &other) const {
            return is_basic != other.is_basic || size != other.size || name != other.name;
        }

        static type boolean() { return {"bool", 1, true}; }
        static type int1() { return {"int1", 1, true}; }
        static type int8() { return {"int8", 1, true}; }
        static type int16() { return {"int16", 2, true}; }
        static type int32() { return {"int32", 4, true}; }
        static type int64() { return {"int64", 8, true}; }
        static type uint8() { return {"uint8", 1, true}; }
        static type uint16() { return {"uint16", 2, true}; }
        static type uint32() { return {"uint32", 4, true}; }
        static type uint64() { return {"uint64", 8, true}; }
    };

}

#endif //SNAKELANG_TYPE_H
