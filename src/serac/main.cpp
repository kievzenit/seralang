#include "CLI11.hpp"
#include "compiler/compiler.h"

int main(int argc, char** argv) {
    CLI::App serac{"Seralang compiler."};
    argv = serac.ensure_utf8(argv);

    compiler::emit_type emit_type;
    std::map<std::string, compiler::emit_type> emit_types = {
            {"none", compiler::emit_type::none},
            {"ir", compiler::emit_type::llvm_ir},
            {"asm", compiler::emit_type::assembler},
            {"obj", compiler::emit_type::object_file},
    };

    bool run_in_jit;

    std::string compile_directory;

    auto emit_option = serac.add_option(
                    "--emit",
                    emit_type,
                    "Emit type: [none, obj, asm, ir].")
            ->default_val(compiler::emit_type::object_file)
            ->transform(CLI::CheckedTransformer(emit_types));

    serac.add_flag(
            "--run-in-jit,!--build",
            run_in_jit,
            "If true, source code runs in JIT.")
            ->default_val(false)
            ->needs(emit_option);

    serac.add_option(
            "--directory,-d",
            compile_directory,
            "Directory that contains source files")
            ->default_val(".")
            ->check(CLI::ExistingDirectory);

    CLI11_PARSE(serac, argc, argv);

    if (run_in_jit && emit_type != compiler::emit_type::none) {
        std::cout << "When running in JIT only none emit type can be used." << std::endl;
        std::exit(-1);
    }

    compiler::compile_options options(compile_directory, emit_type, run_in_jit);
    compiler::compiler compiler(options);
    compiler.compile();

    return 0;
}