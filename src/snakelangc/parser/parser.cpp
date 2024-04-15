#include <memory>
#include "parser.h"

std::unique_ptr<parser::ast::translation_ast> parser::parser::parse() {
    std::vector<std::unique_ptr<ast::top_stmt>> stmts;

    auto package_stmt = parse_package_stmt();

    eat();
    while (current_token_.type != lexer::token_type::eof) {
        auto top_stmt = parse_top_stmt();
        stmts.push_back(std::move(top_stmt));
        eat();
    }

    return std::make_unique<ast::translation_ast>(std::move(package_stmt), std::move(stmts));
}

void parser::parser::expect(lexer::token_type token_type) {
    if (current_token_.type != token_type) {
        unexpected_token_error();
    }
}

void parser::parser::unexpected_token_error() {
    std::string characters_format;
    if (current_token_.column_start != current_token_.column_end) {
        characters_format = std::format("{}-{}", current_token_.column_start, current_token_.column_end);
    } else {
        characters_format = std::to_string(current_token_.column_start);
    }

    auto format = std::format(
            "Unexpected token: {} on line:character {}:{}",
            current_token_.value,
            current_token_.line,
            characters_format);
    utils::log_error(format);
}

void parser::parser::eat() {
    if (!putback_tokens_.empty()) {
        current_token_ = putback_tokens_.top();
        putback_tokens_.pop();
        return;
    }

    current_token_ = lexer_.get_next_token();
}

int parser::parser::get_current_token_precedence() {
    auto binary_operation = current_token_type_to_binary_operation();
    return binary_operation == -1 ? -1 : binop_precedence_[binary_operation];
}

parser::ast::binary_operation parser::parser::current_token_type_to_binary_operation() {
    switch (current_token_.type) {
        case lexer::token_type::plus: return ast::binary_operation::plus;
        case lexer::token_type::minus: return ast::binary_operation::minus;
        case lexer::token_type::asterisk: return ast::binary_operation::multiply;
        case lexer::token_type::slash: return ast::binary_operation::divide;
        default: return (ast::binary_operation)-1;
    }
}

std::unique_ptr<parser::ast::package_stmt> parser::parser::parse_package_stmt() {
    eat();
    expect(lexer::token_type::package);

    std::string package_name;

    eat();
    expect(lexer::token_type::identifier);
    package_name += current_token_.value;

    eat();
    if (current_token_.type == lexer::token_type::semicolon) {
        return std::make_unique<ast::package_stmt>(package_name);
    }

    while (current_token_.type != lexer::token_type::eof) {
        expect(lexer::token_type::dot);
        package_name += current_token_.value;

        eat();
        expect(lexer::token_type::identifier);
        package_name += current_token_.value;

        eat();
        if (current_token_.type == lexer::token_type::semicolon) {
            break;
        }
    }

    return std::make_unique<ast::package_stmt>(package_name);
}

std::unique_ptr<parser::ast::top_stmt> parser::parser::parse_top_stmt() {
    switch (current_token_.type) {
        case lexer::token_type::let: return parse_let_stmt();
        case lexer::token_type::func: return parse_func_decl_stmt();
        default:
            unexpected_token_error();
            __builtin_unreachable();
    }
}

std::unique_ptr<parser::ast::func_decl_stmt> parser::parser::parse_func_decl_stmt() {
    eat();
    expect(lexer::token_type::identifier);

    auto function_name = current_token_.value;

    eat();
    expect(lexer::token_type::l_parenthesis);
    eat();
    expect(lexer::token_type::r_parenthesis);

    eat();
    expect(lexer::token_type::identifier);
    auto return_type = current_token_.value;

    auto scope_stmt = parse_scope_stmt();

    return std::make_unique<ast::func_decl_stmt>(function_name, return_type, std::move(scope_stmt));
}

std::unique_ptr<parser::ast::stmt> parser::parser::parse_stmt() {
    eat();

    switch (current_token_.type) {
        case lexer::token_type::let: return parse_let_stmt();
        case lexer::token_type::ret: return parse_return_stmt();
        default:
            utils::log_error("Unexpected statement got, exiting with error.");
            __builtin_unreachable();
    }
}

std::unique_ptr<parser::ast::scope_stmt> parser::parser::parse_scope_stmt() {
    eat();
    expect(lexer::token_type::l_curly_brace);

    std::vector<std::unique_ptr<ast::stmt>> inner_stmts;

    eat();
    while (current_token_.type != lexer::token_type::r_curly_brace
        && current_token_.type != lexer::token_type::eof) {
        putback_tokens_.push(current_token_);
        inner_stmts.push_back(parse_stmt());
        eat();
    }

    expect(lexer::token_type::r_curly_brace);

    return std::make_unique<ast::scope_stmt>(std::move(inner_stmts));
}

std::unique_ptr<parser::ast::let_stmt> parser::parser::parse_let_stmt() {
    eat();
    expect(lexer::token_type::identifier);
    auto identifier_name = current_token_.value;

    eat();
    expect(lexer::token_type::assign);

    auto expression = parse_expr();

    eat();
    expect(lexer::token_type::semicolon);

    return std::make_unique<ast::let_stmt>(identifier_name, std::move(expression));
}

std::unique_ptr<parser::ast::return_stmt> parser::parser::parse_return_stmt() {
    auto return_stmt = std::make_unique<ast::return_stmt>(parse_expr());

    eat();
    expect(lexer::token_type::semicolon);

    return std::move(return_stmt);
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_expr() {
    auto left = parse_primary_expr();

    return parse_binary_expr(std::move(left), 0);
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_primary_expr() {
    eat();
    switch (current_token_.type) {
        case lexer::token_type::number: return parse_integer_expr();
        case lexer::token_type::boolean: return parse_boolean_expr();
        default:
            unexpected_token_error();
            __builtin_unreachable();
    }
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_binary_expr(
        std::unique_ptr<ast::expr> left,
        int precedence) {
    eat();
    while (true) {
        auto current_precedence = get_current_token_precedence();

        if (current_precedence < precedence) {
            putback_tokens_.push(current_token_);
            return left;
        }

        auto binary_operator = current_token_type_to_binary_operation();

        auto right = parse_primary_expr();

        eat();
        auto next_precedence = get_current_token_precedence();
        if (current_precedence < next_precedence) {
            putback_tokens_.push(current_token_);
            right = parse_binary_expr(std::move(right), current_precedence + 1);
            eat();
        }

        left = std::make_unique<ast::binary_expr>(std::move(left), std::move(right), binary_operator);
    }
}

std::unique_ptr<parser::ast::integer_expr> parser::parser::parse_integer_expr() {
    auto number = std::stoi(current_token_.value);

    eat();
    if (current_token_.type == lexer::token_type::colon) {
        eat();
        expect(lexer::token_type::identifier);

        return std::make_unique<ast::integer_expr>(number, current_token_.value);
    }
    putback_tokens_.push(current_token_);

    return std::make_unique<ast::integer_expr>(number);
}

std::unique_ptr<parser::ast::boolean_expr> parser::parser::parse_boolean_expr() {
    return std::make_unique<ast::boolean_expr>(current_token_.value == "true");
}
