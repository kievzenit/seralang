#ifndef SERALANG_COMPILE_OPTIONS_H
#define SERALANG_COMPILE_OPTIONS_H

#include <string>
#include "emit_type.h"

namespace compiler {

    class compile_options {
    public:
        compile_options() = default;

        explicit compile_options(emit_type output_type) :
            output_type(output_type), run_in_jit(false) {}

        compile_options(std::string current_directory, emit_type output_type) :
            current_directory(std::move(current_directory)), output_type(output_type), run_in_jit(false) {}

        std::string current_directory = std::filesystem::current_path();
        bool run_in_jit = true;
        emit_type output_type = emit_type::none;
    };

}

#endif //SERALANG_COMPILE_OPTIONS_H
