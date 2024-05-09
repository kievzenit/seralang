#include "compiler/compiler.h"

int main() {
    compiler::compile_options options;
    compiler::compiler compiler(options);
    compiler.compile();

    return 0;
}