#ifndef SERALANG_LEXER_H
#define SERALANG_LEXER_H

#include <fstream>
#include <format>
#include <stack>
#include "token.h"
#include "../utils/log_error.h"

namespace lexer {

    class lexer {
    public:
        lexer(lexer& other) :
            stream_(other.stream_.rdbuf()),
            line_(other.line_),
            column_(other.column_),
            current_character_(other.current_character_) {}

        explicit lexer(std::basic_streambuf<char>* streambuf) : stream_(streambuf) {}

        token get_next_token();

    private:
        std::istream stream_;

        int line_ = 1;
        int column_ = 0;

        std::stack<char> putback_characters_;
        char current_character_ = 0;

        void eat_current_char();
        void read_current_char();

        bool can_read();

        bool is_current_char_skippable();
        bool is_current_char_punctuation();
        bool is_current_char_binary_digit();
        bool is_current_char_octal_digit();
        bool is_current_char_decimal_digit();
        bool is_current_char_hexadecimal_digit();
        bool is_current_char_complex_number();
        bool is_current_char_letter();

        void process_skippable();
        void process_comment();
        token process_punctuation();
        token process_number();
        std::string process_complex_number();
        std::string process_binary_number();
        std::string process_octal_number();
        std::string process_decimal_number();
        std::string process_hexadecimal_number();
        token process_identifier();
    };

}

#endif //SERALANG_LEXER_H
