#ifndef SERALANG_INTEGER_TYPE_H
#define SERALANG_INTEGER_TYPE_H

#include "type.h"

namespace emitter::ir {

    class integer_type : public type {
    public:
        bool is_unsigned;

        bool can_be_implicitly_casted_to(type* other) const override;
        bool can_be_explicitly_casted_to(emitter::ir::type *other) const override;

        static integer_type* int1();
        static integer_type* int8();
        static integer_type* int16();
        static integer_type* int32();
        static integer_type* int64();
        static integer_type* uint8();
        static integer_type* uint16();
        static integer_type* uint32();
        static integer_type* uint64();

    private:
        integer_type(std::string name, int size, bool is_unsigned) :
                type(std::move(name), size, true), is_unsigned(is_unsigned) {}

        static integer_type* int1_;
        static integer_type* int8_;
        static integer_type* int16_;
        static integer_type* int32_;
        static integer_type* int64_;
        static integer_type* uint8_;
        static integer_type* uint16_;
        static integer_type* uint32_;
        static integer_type* uint64_;
    };

}

#endif //SERALANG_INTEGER_TYPE_H
