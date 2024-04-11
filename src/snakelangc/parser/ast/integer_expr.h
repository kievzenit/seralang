#ifndef SNAKELANG_INTEGER_EXPR_H
#define SNAKELANG_INTEGER_EXPR_H

namespace parser::ast {

    class integer_expr : public expr {
    public:
        integer_expr(int number) : number(number) {}

        int number;
    };

}

#endif //SNAKELANG_INTEGER_EXPR_H
