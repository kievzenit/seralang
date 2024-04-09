#include <filesystem>
#include "lexer/lexer.h"

int main() {
    std::cout << std::filesystem::current_path() << std::endl;
    lexer::lexer lexer("../../../tests/sources/basic_tokens.sn");
    lexer::token token = lexer.get_next_token();

    do {
        std::cout << (std::string)token << std::endl;
        token = lexer.get_next_token();
    } while (token.type != lexer::token_type::eof);

    std::cout << (std::string)token << std::endl;

    return 0;
}