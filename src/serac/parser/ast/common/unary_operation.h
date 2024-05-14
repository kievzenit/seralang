#ifndef SERALANG_UNARY_OPERATION_H
#define SERALANG_UNARY_OPERATION_H

namespace parser::ast {

    enum unary_operation {
        positive,
        negative,
        logical_not,
        bitwise_not,
        increment,
        decrement,
    };

}

#endif //SERALANG_UNARY_OPERATION_H
