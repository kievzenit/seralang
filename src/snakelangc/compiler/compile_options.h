#ifndef SNAKELANG_COMPILE_OPTIONS_H
#define SNAKELANG_COMPILE_OPTIONS_H

#include <string>
#include "emit_type.h"

namespace compiler {

    class compile_options {
    public:
        std::string current_directory;
        emit_type output_type;
    };

}

#endif //SNAKELANG_COMPILE_OPTIONS_H
