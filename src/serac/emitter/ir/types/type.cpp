#include "type.h"

bool emitter::ir::type::can_be_implicitly_casted_to(type* other) const {
    return false;
}

bool emitter::ir::type::can_be_explicitly_casted_to(emitter::ir::type *other) const {
    return false;
}

emitter::ir::type* emitter::ir::type::boolean_ = nullptr;

emitter::ir::type* emitter::ir::type::boolean() {
    if (!emitter::ir::type::boolean_) {
        emitter::ir::type::boolean_ = new type("bool", 1, true);
    }

    return emitter::ir::type::boolean_;
}
