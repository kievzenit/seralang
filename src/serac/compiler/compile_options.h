#ifndef SERALANG_COMPILE_OPTIONS_H
#define SERALANG_COMPILE_OPTIONS_H

#include <string>
#include "emit_type.h"

namespace compiler {

    class compile_options {
    public:
        std::string current_directory;
        emit_type output_type;
    };

}

#endif //SERALANG_COMPILE_OPTIONS_H
