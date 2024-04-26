#include "compiler/compiler.h"

int main() {
    compiler::compiler compiler({
        std::filesystem::current_path(),
        compiler::emit_type::llvm_ir
    });
    compiler.compile();

    return 0;
}