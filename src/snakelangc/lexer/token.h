#ifndef SNAKELANG_TOKEN_H
#define SNAKELANG_TOKEN_H

#include <string>
#include <map>
#include "token_type.h"

namespace lexer {

    class token {
    public:
        token(token_type type,
              std::string value,
              std::string file_name,
              int line,
              int column_start,
              int column_end) :
              type(type),
              value(std::move(value)),
              file_name(std::move(file_name)),
              line(line), column_start(column_start), column_end(column_end) {}

        explicit operator std::string() {
            return std::format(
                    "token {{ value = {}, line = {}, column_start = {}, column_end = {} }}",
                    value,
                    line,
                    column_start,
                    column_end);
        }

        std::string file_name;

        int line;
        int column_start;
        int column_end;

        token_type type;
        std::string value;
    };

}

#endif //SNAKELANG_TOKEN_H
