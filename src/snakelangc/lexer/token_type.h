#ifndef SNAKELANG_TOKEN_TYPE_H
#define SNAKELANG_TOKEN_TYPE_H

namespace lexer {

    enum token_type {
        eof = 1,

        number,
        boolean,

        identifier,

        let,
        func,
        ret,
        package,
        static_,
        if_,
        else_,
        while_,
        do_,
        switch_,
        for_,
        foreach,
        in,
        break_,
        breakall,

        l_parenthesis,
        r_parenthesis,
        l_bracket,
        r_bracket,
        l_curly_brace,
        r_curly_brace,

        plus,
        minus,
        asterisk,
        percent,
        slash,
        colon,
        semicolon,
        dot,
        coma,
        exclamation_mark,
        bitwise_and,
        bitwise_or,
        bitwise_xor,
        logical_and,
        logical_or,

        assign,
        plus_assign,
        minus_assign,
        multiply_assign,
        divide_assign,
        modulus_assign,

        equals,
        not_equals,
        less_than,
        greater_than,
        less_or_equal,
        greater_or_equal
    };

}

#endif //SNAKELANG_TOKEN_TYPE_H
