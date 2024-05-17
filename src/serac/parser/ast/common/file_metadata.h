#ifndef SERALANG_FILE_METADATA_H
#define SERALANG_FILE_METADATA_H

#include <string>

namespace parser::ast {

    class file_metadata {
    public:
        std::string file_name;

        int line_start;
        int line_end;
        int column_start;
        int column_end;
    };

}

#endif //SERALANG_FILE_METADATA_H
