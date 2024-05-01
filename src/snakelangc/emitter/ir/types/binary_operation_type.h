#ifndef SNAKELANG_BINARY_OPERATION_TYPE_H
#define SNAKELANG_BINARY_OPERATION_TYPE_H

namespace emitter::ir {

    enum binary_operation_type {
        addition,
        subtraction,
        multiplication,
        division,
        modulus,
        equals_to,
        not_equals_to,
        greater_than,
        less_than,
        greater_or_equal,
        less_or_equal,
        bitwise_and,
        bitwise_or,
        bitwise_xor,
        logical_and,
        logical_or,
    };

}

#endif //SNAKELANG_BINARY_OPERATION_TYPE_H
