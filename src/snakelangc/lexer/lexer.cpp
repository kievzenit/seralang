#include "lexer.h"

lexer::token lexer::lexer::get_next_token() {
    while (can_read()) {
        eat_current_char();
        if (is_current_char_skippable()) {
            process_skippable();
            continue;
        }
        if (is_current_char_punctuation()) return process_punctuation();
        if (is_current_char_decimal_digit()) return process_number();
        if (is_current_char_letter() || current_character_ == '_') return process_identifier();
    }

    return {token_type::eof, "eof", file_name_, ++line_, 0, 0};
}

void lexer::lexer::eat_current_char() {
    current_character_ = file_.get();
    column_++;
}

void lexer::lexer::read_current_char() {
    current_character_ = file_.peek();
}

bool lexer::lexer::can_read() {
    return file_.good() && !file_.eof();
}

bool lexer::lexer::is_current_char_skippable() {
    return current_character_ == ' '
        || current_character_ == '\n'
        || current_character_ == '\t'
        || current_character_ == '\r';
}

bool lexer::lexer::is_current_char_punctuation() {
    return current_character_ == '('
        || current_character_ == ')'
        || current_character_ == '['
        || current_character_ == ']'
        || current_character_ == '{'
        || current_character_ == '}'
        || current_character_ == '+'
        || current_character_ == '-'
        || current_character_ == '*'
        || current_character_ == '/'
        || current_character_ == ':'
        || current_character_ == ';'
        || current_character_ == '.'
        || current_character_ == ','
        || current_character_ == '='
        || current_character_ == '<'
        || current_character_ == '>';
}

bool lexer::lexer::is_current_char_binary_digit() {
    return current_character_ >= '0' && current_character_ <= '1';
}

bool lexer::lexer::is_current_char_octal_digit() {
    return is_current_char_binary_digit() || current_character_ >= '2' && current_character_ <= '7';
}

bool lexer::lexer::is_current_char_decimal_digit() {
    return is_current_char_octal_digit() || current_character_ >= '8' && current_character_ <= '9';
}

bool lexer::lexer::is_current_char_hexadecimal_digit() {
    return is_current_char_decimal_digit() || current_character_ >= 'a' && current_character_ <= 'f';
}

bool lexer::lexer::is_current_char_complex_number() {
    return current_character_ == 'b'
        || current_character_ == 'o'
        || current_character_ == 'd'
        || current_character_ == 'x';
}

bool lexer::lexer::is_current_char_letter() {
    return current_character_ >= 'a' && current_character_ <= 'z'
        || current_character_ >= 'A' && current_character_ <= 'Z';
}

void lexer::lexer::process_skippable() {
    if (current_character_ == '\n') {
        line_++;
        column_ = 0;
    }
}

lexer::token lexer::lexer::process_punctuation() {
    char first_char;
    auto column_started = column_;

    switch (current_character_) {
        case '(':
            return {
                    token_type::l_parenthesis,
                    "(",
                    file_name_,
                    line_,
                    column_,
                    column_
            };
        case ')':
            return {
                    token_type::r_parenthesis,
                    ")",
                    file_name_,
                    line_,
                    column_,
                    column_
            };
        case '[':
            return {
                    token_type::l_bracket,
                    "[",
                    file_name_,
                    line_,
                    column_,
                    column_
            };
        case ']':
            return {
                    token_type::r_bracket,
                    "]",
                    file_name_,
                    line_,
                    column_,
                    column_
            };
        case '{':
            return {
                    token_type::l_curly_brace,
                    "{",
                    file_name_,
                    line_,
                    column_,
                    column_
            };
        case '}':
            return {
                    token_type::r_curly_brace,
                    "}",
                    file_name_,
                    line_,
                    column_,
                    column_
            };
        case ':':
            return {
                    token_type::colon,
                    ":",
                    file_name_,
                    line_,
                    column_,
                    column_
            };
        case ';':
            return {
                    token_type::semicolon,
                    ";",
                    file_name_,
                    line_,
                    column_,
                    column_
            };
        case '.':
            return {
                    token_type::dot,
                    ".",
                    file_name_,
                    line_,
                    column_,
                    column_
            };
        case ',':
            return {
                    token_type::coma,
                    ",",
                    file_name_,
                    line_,
                    column_,
                    column_
            };
        case '+':
        case '-':
        case '*':
        case '/':
        case '>':
        case '<':
        case '=':
            first_char = current_character_;
            break;
        default: __builtin_unreachable();
    }

    read_current_char();

    if (current_character_ != '=') {
        switch (first_char) {
            case '+':
                return {
                        token_type::plus,
                        "+",
                        file_name_,
                        line_,
                        column_,
                        column_
                };
            case '-':
                return {
                        token_type::minus,
                        "-",
                        file_name_,
                        line_,
                        column_,
                        column_
                };
            case '*':
                return {
                        token_type::asterisk,
                        "*",
                        file_name_,
                        line_,
                        column_,
                        column_
                };
            case '/':
                return {
                        token_type::slash,
                        "/",
                        file_name_,
                        line_,
                        column_,
                        column_
                };
            case '>':
                return {
                        token_type::greater_than,
                        ">",
                        file_name_,
                        line_,
                        column_,
                        column_
                };
            case '<':
                return {
                        token_type::less_than,
                        "<",
                        file_name_,
                        line_,
                        column_,
                        column_
                };
            case '=':
                return {
                        token_type::assign,
                        "=",
                        file_name_,
                        line_,
                        column_,
                        column_
                };
            default: __builtin_unreachable();
        }
    }

    eat_current_char();
    switch (first_char) {
        case '+':
            return {
                    token_type::plus_assign,
                    "+=",
                    file_name_,
                    line_,
                    column_started,
                    column_
            };
        case '-':
            return {
                    token_type::minus_assign,
                    "-=",
                    file_name_,
                    line_,
                    column_started,
                    column_
            };
        case '*':
            return {
                    token_type::multiply_assign,
                    "*=",
                    file_name_,
                    line_,
                    column_started,
                    column_
            };
        case '/':
            return {
                    token_type::divide_assign,
                    "/=",
                    file_name_,
                    line_,
                    column_started,
                    column_
            };
        case '>':
            return {
                    token_type::greater_or_equal,
                    ">=",
                    file_name_,
                    line_,
                    column_started,
                    column_
            };
        case '<':
            return {
                    token_type::less_or_equal,
                    "<=",
                    file_name_,
                    line_,
                    column_started,
                    column_
            };
        case '=':
            return {
                    token_type::equals,
                    "==",
                    file_name_,
                    line_,
                    column_started,
                    column_
            };
        default: __builtin_unreachable();
    }
}

lexer::token lexer::lexer::process_number() {
    std::string number;
    number += current_character_;

    auto column_started = column_;

    if (current_character_ == '0') {
        read_current_char();

        if (is_current_char_complex_number()) {
            eat_current_char();
            number = process_complex_number();
            return {
                    token_type::number,
                    number,
                    file_name_,
                    line_,
                    column_started,
                    column_
            };
        }
    }

    while (can_read()) {
        read_current_char();

        if (!is_current_char_decimal_digit()) break;

        number += current_character_;
        eat_current_char();
    }

    return {
        token_type::number,
        number,
        file_name_,
        line_,
        column_started,
        column_
    };
}

std::string lexer::lexer::process_complex_number() {
    auto complex_number_identifier = current_character_;

    switch (complex_number_identifier) {
        case 'b': return process_binary_number();
        case 'o': return process_octal_number();
        case 'd': return process_decimal_number();
        case 'x': return process_hexadecimal_number();
        default: __builtin_unreachable();
    }
}

std::string lexer::lexer::process_binary_number() {
    std::string number;

    while (can_read()) {
        read_current_char();

        if (!is_current_char_binary_digit()) break;

        number += current_character_;
        eat_current_char();
    }

    return std::to_string(std::stoi(number, nullptr, 2));
}

std::string lexer::lexer::process_octal_number() {
    std::string number;

    while (can_read()) {
        read_current_char();

        if (!is_current_char_octal_digit()) break;

        number += current_character_;
        eat_current_char();
    }

    return std::to_string(std::stoi(number, nullptr, 8));
}

std::string lexer::lexer::process_decimal_number() {
    std::string number;

    while (can_read()) {
        read_current_char();

        if (!is_current_char_decimal_digit()) break;

        number += current_character_;
        eat_current_char();
    }

    return std::to_string(std::stoi(number));
}

std::string lexer::lexer::process_hexadecimal_number() {
    std::string number;

    while (can_read()) {
        read_current_char();

        if (!is_current_char_hexadecimal_digit()) break;

        number += current_character_;
        eat_current_char();
    }

    return std::to_string(std::stoi(number, nullptr, 16));
}

lexer::token lexer::lexer::process_identifier() {
    std::string identifier;
    identifier += current_character_;

    auto column_started = column_;

    while (can_read()) {
        read_current_char();

        if (!is_current_char_letter() && !is_current_char_decimal_digit() && current_character_ != '_') break;

        identifier += current_character_;
        eat_current_char();
    }

    if (identifier == "let") {
        return {
                token_type::let,
                identifier,
                file_name_,
                line_,
                column_started,
                column_
        };
    }

    if (identifier == "func") {
        return {
                token_type::func,
                identifier,
                file_name_,
                line_,
                column_started,
                column_
        };
    }

    if (identifier == "package") {
        return {
                token_type::package,
                identifier,
                file_name_,
                line_,
                column_started,
                column_
        };
    }

    if (identifier == "true") {
        return {
                token_type::boolean,
                identifier,
                file_name_,
                line_,
                column_started,
                column_
        };
    }

    if (identifier == "false") {
        return {
                token_type::boolean,
                identifier,
                file_name_,
                line_,
                column_started,
                column_
        };
    }

    return {
        token_type::identifier,
        identifier,
        file_name_,
        line_,
        column_started,
        column_
    };
}
