#ifndef SERALANG_TOKEN_H
#define SERALANG_TOKEN_H

#include <string>
#include <map>
#include "token_type.h"

namespace lexer {

    class token {
    public:
        token(token_type type,
              std::string value,
              int line,
              int column_start,
              int column_end) :
              type(type),
              value(std::move(value)),
              line(line), column_start(column_start), column_end(column_end) {}

        token(nullptr_t pVoid) {}

        explicit operator std::string() {
            return std::format(
                    "token {{ value = {}, line = {}, column_start = {}, column_end = {} }}",
                    value,
                    line,
                    column_start,
                    column_end);
        }

        int line;
        int column_start;
        int column_end;

        token_type type;
        std::string value;
    };

}

#endif //SERALANG_TOKEN_H
