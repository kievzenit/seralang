#ifndef SERALANG_ERROR_H
#define SERALANG_ERROR_H

#include <string>

namespace errors {

    class error {
    public:
        error(
                std::string error_message,
                std::string hint_message,
                std::string file_name,
                int line,
                int column_start,
                int column_end) :
                error_message(std::move(error_message)),
                hint_message(std::move(hint_message)),
                file_name(std::move(file_name)),
                line(line),
                column_start(column_start),
                column_end(column_end) {}

        std::string error_message;
        std::string hint_message;

        std::string file_name;
        int line;
        int column_start;
        int column_end;
    };

}

#endif //SERALANG_ERROR_H
