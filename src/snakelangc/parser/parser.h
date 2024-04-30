#ifndef SNAKELANG_PARSER_H
#define SNAKELANG_PARSER_H

#include <string>
#include <memory>
#include <utility>
#include <stack>
#include "../lexer/lexer.h"
#include "ast/translation_ast.h"
#include "ast/stmts/let_stmt.h"
#include "ast/exprs/integer_expr.h"
#include "ast/exprs/boolean_expr.h"
#include "ast/exprs/binary_expr.h"
#include "ast/common/binary_operation.h"
#include "ast/stmts/func_decl_stmt.h"
#include "ast/stmts/return_stmt.h"
#include "ast/stmts/call_stmt.h"
#include "ast/exprs/identifier_expr.h"
#include "ast/stmts/assignment_stmt.h"

namespace parser {

    class parser {
    public:
        explicit parser(lexer::lexer lexer) : lexer_(lexer) {}

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
                {ast::binary_operation::modulus, 20},
        };

        bool has_tokens() const;

        void expect(lexer::token_type token_type);
        void unexpected_token_error();
        void eat();

        int get_current_token_precedence();
        ast::binary_operation current_token_type_to_binary_operation();
        ast::binary_operation token_type_to_binary_operation(lexer::token_type token_type);

        std::unique_ptr<ast::package_stmt> parse_package_stmt();
        std::unique_ptr<ast::top_stmt> parse_top_stmt();
        std::unique_ptr<ast::func_decl_stmt> parse_func_decl_stmt();
        std::vector<ast::func_param> parse_func_params();
        ast::func_param parse_func_param();
        std::unique_ptr<ast::stmt> parse_stmt();
        std::unique_ptr<ast::scope_stmt> parse_scope_stmt();
        std::unique_ptr<ast::let_stmt> parse_let_stmt(bool is_static);
        std::unique_ptr<ast::assignment_stmt> parse_assignment_stmt();
        std::unique_ptr<ast::call_stmt> parse_call_stmt();
        std::unique_ptr<ast::return_stmt> parse_return_stmt();

        std::unique_ptr<ast::expr> parse_expr();
        std::unique_ptr<ast::expr> parse_primary_expr();
        std::unique_ptr<ast::expr> parse_binary_expr(std::unique_ptr<ast::expr> left, int precedence = 0);
        std::unique_ptr<ast::call_expr> parse_call_expr();
        std::vector<std::unique_ptr<ast::expr>> parse_call_arguments();
        std::unique_ptr<ast::expr> parse_identifier_expr();
        std::unique_ptr<ast::integer_expr> parse_integer_expr();
        std::unique_ptr<ast::boolean_expr> parse_boolean_expr();
    };

}

#endif //SNAKELANG_PARSER_H
