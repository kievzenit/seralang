#include "integer_type.h"

bool emitter::ir::integer_type::can_be_implicitly_casted_to(type* other) const {
    if (this == int1()) {
        return other == int8()
               || other == uint16()
               || other == int16()
               || other == uint16()
               || other == int32()
               || other == uint32()
               || other == int64()
               || other == uint64();
    }

    if (this == int8()) {
        return other == uint8()
               || other == int16()
               || other == uint16()
               || other == int32()
               || other == int64()
               || other == uint32()
               || other == uint64();
    }

    if (this == uint8()) {
        return other == int8()
               || other == uint16()
               || other == int16()
               || other == int64()
               || other == int32()
               || other == uint64()
               || other == uint32();
    }

    if (this == int16()) {
        return other == int32()
               || other == uint32()
               || other == int64()
               || other == uint64();
    }

    if (this == uint16()) {
        return other == int16()
               || other == uint32()
               || other == int32()
               || other == uint64()
               || other == int64();
    }

    if (this == int32()) {
        return other == int64() || other == uint64();
    }

    if (this == uint32()) {
        return other == int32()
               || other == uint64()
               || other == int64();
    }

    if (this == int64()) {
        return other == uint64();
    }

    if (this == uint64()) {
        return other == int64();
    }

    return false;
}

bool emitter::ir::integer_type::can_be_explicitly_casted_to(emitter::ir::type *other) const {
    return this == int1()
           || this == int8()
           || this == int16()
           || this == int32()
           || this == int64()
           || this == uint8()
           || this == uint16()
           || this == uint32()
           || this == uint64();
}

emitter::ir::integer_type* emitter::ir::integer_type::int1_ = nullptr;
emitter::ir::integer_type* emitter::ir::integer_type::int8_ = nullptr;
emitter::ir::integer_type* emitter::ir::integer_type::int16_ = nullptr;
emitter::ir::integer_type* emitter::ir::integer_type::int32_ = nullptr;
emitter::ir::integer_type* emitter::ir::integer_type::int64_ = nullptr;
emitter::ir::integer_type* emitter::ir::integer_type::uint8_ = nullptr;
emitter::ir::integer_type* emitter::ir::integer_type::uint16_ = nullptr;
emitter::ir::integer_type* emitter::ir::integer_type::uint32_ = nullptr;
emitter::ir::integer_type* emitter::ir::integer_type::uint64_ = nullptr;

emitter::ir::integer_type* emitter::ir::integer_type::int1() {
    if (!int1_) {
        int1_ = new integer_type("int1", 1, false);
    }

    return int1_;
}

emitter::ir::integer_type* emitter::ir::integer_type::int8() {
    if (!int8_) {
        int8_ = new integer_type("int8", 1, false);
    }

    return int8_;
}

emitter::ir::integer_type* emitter::ir::integer_type::int16() {
    if (!int16_) {
        int16_ = new integer_type("int16", 2, false);
    }

    return int16_;
}

emitter::ir::integer_type* emitter::ir::integer_type::int32() {
    if (!int32_) {
        int32_ = new integer_type("int32", 4, false);
    }

    return int32_;
}

emitter::ir::integer_type* emitter::ir::integer_type::int64() {
    if (!int64_) {
        int64_ = new integer_type("int64", 8, false);
    }

    return int64_;
}

emitter::ir::integer_type* emitter::ir::integer_type::uint8() {
    if (!uint8_) {
        uint8_ = new integer_type("uint8", 1, true);
    }

    return uint8_;
}

emitter::ir::integer_type* emitter::ir::integer_type::uint16() {
    if (!uint16_) {
        uint16_ = new integer_type("uint16", 2, true);
    }

    return uint16_;
}

emitter::ir::integer_type* emitter::ir::integer_type::uint32() {
    if (!uint32_) {
        uint32_ = new integer_type("uint32", 4, true);
    }

    return uint32_;
}

emitter::ir::integer_type* emitter::ir::integer_type::uint64() {
    if (!uint64_) {
        uint64_ = new integer_type("uint64", 8, true);
    }

    return uint64_;
}
