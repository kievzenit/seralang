#ifndef SNAKELANG_SCOPE_STMT_IR_H
#define SNAKELANG_SCOPE_STMT_IR_H

#include <vector>
#include <memory>
#include "stmt_ir.h"

namespace emitter::ir {

    class scope_stmt_ir : public stmt_ir {
    public:
        scope_stmt_ir(std::vector<std::unique_ptr<stmt_ir>> inner_stmts) : inner_stmts(std::move(inner_stmts)) {}

        std::vector<std::unique_ptr<stmt_ir>> inner_stmts;
    };

}

#endif //SNAKELANG_SCOPE_STMT_IR_H
