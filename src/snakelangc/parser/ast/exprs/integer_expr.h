#include <utility>

#ifndef SNAKELANG_INTEGER_EXPR_H
#define SNAKELANG_INTEGER_EXPR_H

namespace parser::ast {

    class integer_expr : public expr {
    public:
        integer_expr(int number, std::string explicit_int_type = "") :
            number(number), explicit_int_type(std::move(explicit_int_type)) {}

        int number;
        std::string explicit_int_type;
    };

}

#endif //SNAKELANG_INTEGER_EXPR_H
