#include <filesystem>
#include "lexer/lexer.h"
#include "parser/parser.h"

int main() {
    const auto file_name = "../../../tests/sources/basic_tokens.sn";
    parser::parser parser(file_name);
    auto translation_ast = parser.parse();

    for (auto &stmt : translation_ast->stmts) {
        auto let_stmt = dynamic_cast<parser::ast::let_stmt*>(stmt.get());
    }

    return 0;
}