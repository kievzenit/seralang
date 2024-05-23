#ifndef SERALANG_TOKEN_TYPE_H
#define SERALANG_TOKEN_TYPE_H

#include <map>
#include <string>

namespace lexer {

    enum token_type {
        eof = 1,

        integer,
        boolean,

        identifier,

        let,
        func,
        ret,
        package,
        static_,
        extern_,
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
        continue_,
        loop,

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
        bitwise_not,
        cast,

        plus_plus,
        minus_minus,
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
        greater_or_equal,
    };

    std::string convert_token_type_to_string(const lexer::token_type& type);

}

#endif //SERALANG_TOKEN_TYPE_H
