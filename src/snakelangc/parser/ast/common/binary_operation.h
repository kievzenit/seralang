#ifndef SNAKELANG_BINARY_OPERATION_H
#define SNAKELANG_BINARY_OPERATION_H

namespace parser::ast {

    enum binary_operation {
        // arithmetic
        plus,
        minus,
        multiply,
        divide,
        modulus,

        // relational
        equals_to,
        not_equals_to,
        greater_than,
        less_than,
        greater_or_equal,
        less_or_equal,

        // bitwise,
        bitwise_and,
        bitwise_or,
        bitwise_xor,

        // logical
        logical_and,
        logical_or
    };

}

#endif //SNAKELANG_BINARY_OPERATION_H
