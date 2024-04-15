#ifndef SNAKELANG_TYPE_H
#define SNAKELANG_TYPE_H

#include <string>

namespace emitter::ir {

    class type {
    public:
        type(std::string name, bool is_basic = false) : name(std::move(name)), is_basic(is_basic) {}

        std::string name;
        bool is_basic;

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
            return is_basic == other.is_basic && name == other.name;
        }

        bool operator!=(const type &other) const {
            return is_basic != other.is_basic || name != other.name;
        }

        static type boolean() { return {"bool", true}; }
        static type int1() { return {"int1", true}; }
        static type int8() { return {"int8", true}; }
        static type int16() { return {"int16", true}; }
        static type int32() { return {"int32", true}; }
        static type int64() { return {"int64", true}; }
        static type uint8() { return {"uint8", true}; }
        static type uint16() { return {"uint16", true}; }
        static type uint32() { return {"uint32", true}; }
        static type uint64() { return {"uint64", true}; }
    };

}

#endif //SNAKELANG_TYPE_H
