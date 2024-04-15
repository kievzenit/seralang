#include "parser/parser.h"
#include "emitter/emitter.h"
#include "translator/translator.h"

int main() {
    const auto file_name = "../../../tests/sources/basic_tokens.sn";

    parser::parser parser(file_name);
    auto translation_ast = parser.parse();

    emitter::emitter emitter(std::move(translation_ast));
    auto package_ir = emitter.emit();

    translator::translator translator(std::move(package_ir));
    translator.translate();

    return 0;
}