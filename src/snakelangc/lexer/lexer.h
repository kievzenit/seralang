#ifndef SNAKELANG_LEXER_H
#define SNAKELANG_LEXER_H

#include <fstream>
#include <format>
#include "token.h"
#include "../utils/log_error.h"

namespace lexer {

    class lexer {
    public:
        lexer(lexer& lexer) {
            file_name_ = lexer.file_name_;
            file_.swap(lexer.file_);
            line_ = lexer.line_;
            column_ = lexer.column_;
            current_character_ = lexer.current_character_;
        }

        explicit lexer(std::string file_name) : file_name_(std::move(file_name)) {
            file_ = std::ifstream(file_name_);

            if (!file_.is_open()) {
                auto error_message = std::format(
                        "Unable to open file_: {}, exiting with error.\n",
                        file_name_);
                utils::log_error(error_message);
            }
        }

        token get_next_token();
    private:
        std::string file_name_;
        std::ifstream file_;

        int line_ = 1;
        int column_ = 0;

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

#endif //SNAKELANG_LEXER_H
