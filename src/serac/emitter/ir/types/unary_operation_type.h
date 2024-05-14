#ifndef SERALANG_UNARY_OPERATION_TYPE_H
#define SERALANG_UNARY_OPERATION_TYPE_H

namespace emitter::ir {

    enum unary_operation_type {
        positive,
        negative,
        logical_not,
        bitwise_not,
        increment,
        decrement,
    };

}

#endif //SERALANG_UNARY_OPERATION_TYPE_H
