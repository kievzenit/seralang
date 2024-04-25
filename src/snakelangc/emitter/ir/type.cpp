#include "type.h"

bool emitter::ir::type::can_be_explicitly_casted_to(type* other) const {
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
        return other == int16()
               || other == int32()
               || other == int64();
    }

    if (this == int16()) {
        return other == int32()
               || other == int64();
    }

    if (this == int32()) {
        return other == int64();
    }

    return false;
}

emitter::ir::type* emitter::ir::type::boolean_ = nullptr;
emitter::ir::type* emitter::ir::type::int1_ = nullptr;
emitter::ir::type* emitter::ir::type::int8_ = nullptr;
emitter::ir::type* emitter::ir::type::int16_ = nullptr;
emitter::ir::type* emitter::ir::type::int32_ = nullptr;
emitter::ir::type* emitter::ir::type::int64_ = nullptr;
emitter::ir::type* emitter::ir::type::uint8_ = nullptr;
emitter::ir::type* emitter::ir::type::uint16_ = nullptr;
emitter::ir::type* emitter::ir::type::uint32_ = nullptr;
emitter::ir::type* emitter::ir::type::uint64_ = nullptr;

emitter::ir::type* emitter::ir::type::boolean() {
    if (!emitter::ir::type::boolean_) {
        emitter::ir::type::boolean_ = new type("bool", 1, true);
    }

    return emitter::ir::type::boolean_;
}

emitter::ir::type* emitter::ir::type::int1() {
    if (!int1_) {
        int1_ = new type("int1", 1, true);
    }

    return int1_;
}

emitter::ir::type* emitter::ir::type::int8() {
    if (!int8_) {
        int8_ = new type("int8", 1, true);
    }

    return int8_;
}

emitter::ir::type* emitter::ir::type::int16() {
    if (!int16_) {
        int16_ = new type("int16", 2, true);
    }

    return int16_;
}

emitter::ir::type* emitter::ir::type::int32() {
    if (!int32_) {
        int32_ = new type("int32", 4, true);
    }

    return int32_;
}

emitter::ir::type* emitter::ir::type::int64() {
    if (!int64_) {
        int64_ = new type("int64", 8, true);
    }

    return int64_;
}

emitter::ir::type* emitter::ir::type::uint8() {
    if (!uint8_) {
        uint8_ = new type("uint8", 1, true, true);
    }

    return uint8_;
}

emitter::ir::type* emitter::ir::type::uint16() {
    if (!uint16_) {
        uint16_ = new type("uint16", 2, true, true);
    }

    return uint16_;
}

emitter::ir::type* emitter::ir::type::uint32() {
    if (!uint32_) {
        uint32_ = new type("uint32", 4, true, true);
    }

    return uint32_;
}

emitter::ir::type* emitter::ir::type::uint64() {
    if (!uint64_) {
        uint64_ = new type("uint64", 8, true, true);
    }

    return uint64_;
}