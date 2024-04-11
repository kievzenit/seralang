#ifndef SNAKELANG_PARSER_H
#define SNAKELANG_PARSER_H

#include <string>
#include <utility>
#include <stack>
#include "../lexer/lexer.h"
#include "ast/translation_ast.h"
#include "ast/let_stmt.h"
#include "ast/integer_expr.h"
#include "ast/boolean_expr.h"
#include "ast/binary_expr.h"
#include "ast/binary_operation.h"

namespace parser {

    class parser {
    public:
        parser(std::string file_name) : lexer_(std::move(file_name)) {}

        std::unique_ptr<ast::translation_ast> parse();
    private:
        lexer::lexer lexer_;
        lexer::token current_token_ = nullptr;
        std::stack<lexer::token> putback_tokens_;

        std::map<ast::binary_operation, int> binop_precedence_ = {
                {ast::binary_operation::plus, 10},
                {ast::binary_operation::minus, 10},
                {ast::binary_operation::multiply, 20},
                {ast::binary_operation::divide, 20},
        };

        void expect(lexer::token_type token_type);
        void unexpected_token_error();
        void eat();

        int get_current_token_precedence();
        ast::binary_operation current_token_type_to_binary_operation();

        std::unique_ptr<ast::package_stmt> parse_package_stmt();
        std::unique_ptr<ast::top_stmt> parse_top_stmt();
        std::unique_ptr<ast::let_stmt> parse_let_stmt();

        std::unique_ptr<ast::expr> parse_expr();
        std::unique_ptr<ast::expr> parse_primary_expr();
        std::unique_ptr<ast::expr> parse_binary_expr(std::unique_ptr<ast::expr> left, int precedence = 0);
        std::unique_ptr<ast::integer_expr> parse_integer_expr();
        std::unique_ptr<ast::boolean_expr> parse_boolean_expr();
    };

}

#endif //SNAKELANG_PARSER_H