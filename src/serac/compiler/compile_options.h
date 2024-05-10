#ifndef SERALANG_COMPILE_OPTIONS_H
#define SERALANG_COMPILE_OPTIONS_H

#include <string>
#include "emit_type.h"

namespace compiler {

    class compile_options {
    public:
        compile_options() = default;

        compile_options(std::string current_directory, emit_type output_type, bool run_in_jit) :
            current_directory(std::move(current_directory)), output_type(output_type), run_in_jit(run_in_jit) {}

        std::string current_directory = std::filesystem::current_path();
        bool run_in_jit = false;
        emit_type output_type = emit_type::object_file;
    };

}

#endif //SERALANG_COMPILE_OPTIONS_H
