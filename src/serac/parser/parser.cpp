#include "parser.h"

std::unique_ptr<parser::ast::translation_ast> parser::parser::parse() {
    std::vector<std::unique_ptr<ast::top_stmt>> stmts;

    auto package_stmt = parse_package_stmt();

    eat();
    while (has_tokens()) {
        auto top_stmt = parse_top_stmt();
        stmts.push_back(std::move(top_stmt));
        eat();
    }

    return std::make_unique<ast::translation_ast>(std::move(package_stmt), std::move(stmts));
}

bool parser::parser::has_tokens() const {
    return current_token_.type != lexer::token_type::eof;
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

bool parser::parser::is_current_token_assignment_token() const {
    switch (current_token_.type) {
        case lexer::token_type::assign:
        case lexer::token_type::plus_assign:
        case lexer::token_type::minus_assign:
        case lexer::token_type::multiply_assign:
        case lexer::token_type::divide_assign:
        case lexer::token_type::modulus_assign:
            return true;
        default:
            return false;
    }
}

int parser::parser::get_current_token_precedence() {
    auto binary_operation = current_token_type_to_binary_operation();
    return binary_operation == -1 ? -1 : binop_precedence_[binary_operation];
}

parser::ast::binary_operation parser::parser::current_token_type_to_binary_operation() {
    return token_type_to_binary_operation(current_token_.type);
}

parser::ast::binary_operation parser::parser::token_type_to_binary_operation(lexer::token_type token_type) {
    switch (token_type) {
        case lexer::token_type::plus: return ast::binary_operation::plus;
        case lexer::token_type::minus: return ast::binary_operation::minus;
        case lexer::token_type::asterisk: return ast::binary_operation::multiply;
        case lexer::token_type::slash: return ast::binary_operation::divide;
        case lexer::token_type::percent: return ast::binary_operation::modulus;
        case lexer::token_type::plus_assign: return ast::binary_operation::plus;
        // TODO: rethink this
        case lexer::token_type::minus_assign: return ast::binary_operation::minus;
        case lexer::token_type::multiply_assign: return ast::binary_operation::multiply;
        case lexer::token_type::divide_assign: return ast::binary_operation::divide;
        case lexer::token_type::modulus_assign: return ast::binary_operation::modulus;
        // ---------
        case lexer::token_type::equals: return ast::binary_operation::equals_to;
        case lexer::token_type::not_equals: return ast::binary_operation::not_equals_to;
        case lexer::token_type::greater_than: return ast::binary_operation::greater_than;
        case lexer::token_type::less_than: return ast::binary_operation::less_than;
        case lexer::token_type::greater_or_equal: return ast::binary_operation::greater_or_equal;
        case lexer::token_type::less_or_equal: return ast::binary_operation::less_or_equal;
        case lexer::token_type::logical_and: return ast::binary_operation::logical_and;
        case lexer::token_type::logical_or: return ast::binary_operation::logical_or;
        case lexer::token_type::bitwise_and: return ast::binary_operation::bitwise_and;
        case lexer::token_type::bitwise_or: return ast::binary_operation::bitwise_or;
        case lexer::token_type::bitwise_xor: return ast::binary_operation::bitwise_xor;
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

    while (has_tokens()) {
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
        case lexer::token_type::static_: eat(); return parse_let_stmt(true);
        case lexer::token_type::let: return parse_let_stmt(false);
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

    auto params = parse_func_params();

    eat();
    expect(lexer::token_type::identifier);
    auto return_type = current_token_.value;

    auto scope_stmt = parse_scope_stmt();

    return std::make_unique<ast::func_decl_stmt>(
            function_name, params, return_type, std::move(scope_stmt));
}

std::vector<parser::ast::func_param> parser::parser::parse_func_params() {
    std::vector<ast::func_param> params;

    eat();
    expect(lexer::token_type::l_parenthesis);

    eat();
    do {
        if (current_token_.type == lexer::token_type::r_parenthesis) {
            break;
        }

        putback_tokens_.push(current_token_);
        params.push_back(parse_func_param());

        eat();
        if (current_token_.type == lexer::token_type::coma) {
            eat();
        }
    } while (has_tokens());

    return params;
}

parser::ast::func_param parser::parser::parse_func_param() {
    eat();
    expect(lexer::token_type::identifier);
    auto param_name = current_token_.value;

    eat();
    expect(lexer::token_type::colon);

    eat();
    expect(lexer::token_type::identifier);
    auto param_type = current_token_.value;

    return {param_name, param_type};
}

std::unique_ptr<parser::ast::stmt> parser::parser::parse_stmt() {
    eat();

    switch (current_token_.type) {
        case lexer::token_type::if_:
        case lexer::token_type::while_:
        case lexer::token_type::do_:
        case lexer::token_type::loop:
        case lexer::token_type::for_:
            return parse_compound_stmt();
        case lexer::token_type::break_:
        case lexer::token_type::breakall:
        case lexer::token_type::continue_:
        case lexer::token_type::ret:
            return parse_control_flow_stmt();
        case lexer::token_type::let:
        case lexer::token_type::static_:
        case lexer::token_type::identifier:
            return parse_local_stmt();
        case lexer::token_type::l_curly_brace:
            putback_tokens_.push(current_token_);
            return parse_scope_stmt();
        default:
            utils::log_error("Unexpected statement got, exiting with error from parser.");
            __builtin_unreachable();
    }
}

std::unique_ptr<parser::ast::scope_stmt> parser::parser::parse_scope_stmt() {
    eat();
    expect(lexer::token_type::l_curly_brace);

    std::vector<std::unique_ptr<ast::stmt>> inner_stmts;

    eat();
    while (has_tokens() && current_token_.type != lexer::token_type::r_curly_brace) {
        putback_tokens_.push(current_token_);
        inner_stmts.push_back(parse_stmt());
        eat();
    }

    expect(lexer::token_type::r_curly_brace);

    return std::make_unique<ast::scope_stmt>(std::move(inner_stmts));
}

std::unique_ptr<parser::ast::compound_stmt> parser::parser::parse_compound_stmt() {
    switch (current_token_.type) {
        case lexer::token_type::if_:
            return parse_if_stmt();
        case lexer::token_type::while_:
            return parse_while_stmt();
        case lexer::token_type::do_:
            return parse_do_while_stmt();
        case lexer::token_type::loop:
            return parse_loop_stmt();
        case lexer::token_type::for_:
            return parse_for_stmt();
        default:
            utils::log_error("Unexpected compound statement got, exiting with error from parser.");
            __builtin_unreachable();
    }
}

std::unique_ptr<parser::ast::if_stmt> parser::parser::parse_if_stmt() {
    expect(lexer::token_type::if_);

    auto if_expr = parse_parenthesis_expr();
    auto if_scope = parse_scope_stmt();

    std::vector<std::unique_ptr<ast::else_if_stmt>> else_if_branches;
    std::unique_ptr<ast::else_stmt> else_branch;

    eat();
    auto else_token = current_token_;
    if (current_token_.type != lexer::token_type::else_) {
        putback_tokens_.push(else_token);

        return std::make_unique<ast::if_stmt>(
                std::move(if_expr),
                std::move(if_scope),
                std::move(else_if_branches),
                std::move(else_branch));
    }

    eat();
    if (current_token_.type == lexer::token_type::if_) {
        putback_tokens_.push(current_token_);
        putback_tokens_.push(else_token);

        else_if_branches = parse_else_if_stmts();
    } else {
        putback_tokens_.push(current_token_);
        putback_tokens_.push(else_token);
    }

    eat();
    if (current_token_.type != lexer::token_type::else_) {
        putback_tokens_.push(current_token_);

        return std::make_unique<ast::if_stmt>(
                std::move(if_expr),
                std::move(if_scope),
                std::move(else_if_branches),
                std::move(else_branch));
    }

    putback_tokens_.push(current_token_);
    else_branch = parse_else_stmt();

    return std::make_unique<ast::if_stmt>(
            std::move(if_expr),
            std::move(if_scope),
            std::move(else_if_branches),
            std::move(else_branch));
}

std::vector<std::unique_ptr<parser::ast::else_if_stmt>> parser::parser::parse_else_if_stmts() {
    std::vector<std::unique_ptr<ast::else_if_stmt>> else_if_branches;

    eat();
    while (has_tokens() && current_token_.type == lexer::token_type::else_) {
        auto else_token = current_token_;
        eat();
        if (current_token_.type != lexer::token_type::if_) {
            putback_tokens_.push(current_token_);
            current_token_ = else_token;
            break;
        }

        auto else_if_stmt = parse_else_if_stmt();
        else_if_branches.push_back(std::move(else_if_stmt));
        eat();
    }

    putback_tokens_.push(current_token_);

    return else_if_branches;
}

std::unique_ptr<parser::ast::else_if_stmt> parser::parser::parse_else_if_stmt() {
    expect(lexer::token_type::if_);

    auto else_if_expr = parse_parenthesis_expr();
    auto else_if_scope = parse_scope_stmt();

    return std::make_unique<ast::else_if_stmt>(std::move(else_if_expr), std::move(else_if_scope));
}

std::unique_ptr<parser::ast::else_stmt> parser::parser::parse_else_stmt() {
    eat();
    expect(lexer::token_type::else_);

    auto else_scope = parse_scope_stmt();

    return std::make_unique<ast::else_stmt>(std::move(else_scope));
}

std::unique_ptr<parser::ast::while_stmt> parser::parser::parse_while_stmt() {
    expect(lexer::token_type::while_);

    auto condition_expr = parse_parenthesis_expr();
    auto while_scope = parse_scope_stmt();

    return std::make_unique<ast::while_stmt>(std::move(condition_expr), std::move(while_scope));
}

std::unique_ptr<parser::ast::do_while_stmt> parser::parser::parse_do_while_stmt() {
    expect(lexer::token_type::do_);

    auto do_while_scope = parse_scope_stmt();

    eat();
    expect(lexer::token_type::while_);

    auto condition_expr = parse_parenthesis_expr();

    eat();
    expect(lexer::token_type::semicolon);

    return std::make_unique<ast::do_while_stmt>(std::move(condition_expr), std::move(do_while_scope));
}

std::unique_ptr<parser::ast::loop_stmt> parser::parser::parse_loop_stmt() {
    expect(lexer::token_type::loop);

    auto scope = parse_scope_stmt();

    return std::make_unique<ast::loop_stmt>(std::move(scope));
}

std::unique_ptr<parser::ast::for_stmt> parser::parser::parse_for_stmt() {
    expect(lexer::token_type::for_);

    eat();
    expect(lexer::token_type::l_parenthesis);

    std::vector<std::unique_ptr<ast::stmt>> run_once;
    while (current_token_.type != lexer::token_type::semicolon) {
        auto stmt = parse_assignment_stmts(false);
        run_once.push_back(std::move(stmt));

        if (current_token_.type != lexer::token_type::coma
            && current_token_.type != lexer::token_type::semicolon) {
            expect(lexer::token_type::coma);
        }
    }

    auto condition = parse_expr();
    eat();
    expect(lexer::token_type::semicolon);

    std::vector<std::unique_ptr<ast::expr>> run_after_each;
    while (current_token_.type != lexer::token_type::r_parenthesis) {
        auto stmt = parse_expr();
        run_after_each.push_back(std::move(stmt));
        eat();

        if (current_token_.type != lexer::token_type::coma
            && current_token_.type != lexer::token_type::r_parenthesis) {
            expect(lexer::token_type::coma);
        }
    }

    auto scope = parse_scope_stmt();

    return std::make_unique<ast::for_stmt>(
            std::move(run_once), std::move(condition), std::move(run_after_each), std::move(scope));
}

std::unique_ptr<parser::ast::local_stmt> parser::parser::parse_local_stmt() {
    auto priv_token = current_token_;
    switch (current_token_.type) {
        case lexer::token_type::let:
        case lexer::token_type::static_:
            putback_tokens_.push(current_token_);
            return parse_assignment_stmts(true);
        case lexer::token_type::identifier:
            eat();

            putback_tokens_.push(current_token_);
            putback_tokens_.push(priv_token);

            if (current_token_.type == lexer::token_type::l_parenthesis) {
                return parse_call_stmt();
            }

            eat();
            return parse_assignment_stmt();
        default:
            utils::log_error("Unexpected local statement got, exiting with error from parser.");
            __builtin_unreachable();
    }
}

std::unique_ptr<parser::ast::local_stmt> parser::parser::parse_assignment_stmts(bool expect_semicolon) {
    eat();
    switch (current_token_.type) {
        case lexer::token_type::let:
            return parse_let_stmt(false, expect_semicolon);
        case lexer::token_type::static_:
            eat();
            return parse_let_stmt(true, expect_semicolon);
        case lexer::token_type::identifier:
            return parse_assignment_stmt(expect_semicolon);
        default:
            utils::log_error("Unexpected assignment statement got, exiting with error from parser.");
            __builtin_unreachable();
    }
}

std::unique_ptr<parser::ast::call_stmt> parser::parser::parse_call_stmt() {
        auto expr = parse_call_expr();

        eat();
        expect(lexer::token_type::semicolon);

        return std::make_unique<ast::call_stmt>(std::move(expr));
    }

std::unique_ptr<parser::ast::let_stmt> parser::parser::parse_let_stmt(bool is_static, bool expect_semicolon) {
    eat();
    expect(lexer::token_type::identifier);
    auto identifier_name = current_token_.value;

    eat();
    expect(lexer::token_type::assign);

    auto expression = parse_expr();

    eat();
    if (expect_semicolon) {
        expect(lexer::token_type::semicolon);
    }

    return std::make_unique<ast::let_stmt>(identifier_name, std::move(expression), is_static);
}

std::unique_ptr<parser::ast::assignment_stmt> parser::parser::parse_assignment_stmt(bool expect_semicolon) {
    putback_tokens_.push(current_token_);
    auto expr = parse_assignment_expr();

    eat();
    if (expect_semicolon) {
        expect(lexer::token_type::semicolon);
    }

    return std::make_unique<ast::assignment_stmt>(std::move(expr));
}

std::unique_ptr<parser::ast::control_flow_stmt> parser::parser::parse_control_flow_stmt() {
    switch (current_token_.type) {
        case lexer::token_type::break_:
            return parse_break_stmt();
        case lexer::token_type::breakall:
            return parse_breakall_stmt();
        case lexer::token_type::continue_:
            return parse_continue_stmt();
        case lexer::token_type::ret:
            return parse_return_stmt();
        default:
            utils::log_error("Unexpected statement got, exiting with error.");
            __builtin_unreachable();
    }
}

std::unique_ptr<parser::ast::return_stmt> parser::parser::parse_return_stmt() {
    auto return_stmt = std::make_unique<ast::return_stmt>(parse_expr());

    eat();
    expect(lexer::token_type::semicolon);

    return std::move(return_stmt);
}

std::unique_ptr<parser::ast::break_stmt> parser::parser::parse_break_stmt() {
    expect(lexer::token_type::break_);

    eat();
    if (current_token_.type == lexer::token_type::semicolon) {
        return std::make_unique<ast::break_stmt>();
    }

    putback_tokens_.push(current_token_);
    auto break_expr = parse_expr();

    eat();
    expect(lexer::token_type::semicolon);

    return std::make_unique<ast::break_stmt>(std::move(break_expr));
}

std::unique_ptr<parser::ast::breakall_stmt> parser::parser::parse_breakall_stmt() {
    expect(lexer::token_type::breakall);

    eat();
    expect(lexer::token_type::semicolon);

    return std::make_unique<ast::breakall_stmt>();
}

std::unique_ptr<parser::ast::continue_stmt> parser::parser::parse_continue_stmt() {
    expect(lexer::token_type::continue_);

    eat();
    expect(lexer::token_type::semicolon);

    return std::make_unique<ast::continue_stmt>();
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_expr() {
    auto left = parse_unary_expr();

    return parse_binary_expr(std::move(left), 0);
}

std::unique_ptr<parser::ast::assignment_expr> parser::parser::parse_assignment_expr() {
    eat();
    expect(lexer::token_type::identifier);
    auto identifier_name = current_token_.value;

    eat();
    auto assign_token_type = current_token_.type;
    if (!is_current_token_assignment_token()) {
        unexpected_token_error();
    }

    auto expr = parse_expr();

    if (assign_token_type != lexer::token_type::assign) {
        expr = std::make_unique<ast::binary_expr>(
                std::make_unique<ast::identifier_expr>(identifier_name),
                std::move(expr),
                token_type_to_binary_operation(assign_token_type));
    }

    return std::make_unique<ast::assignment_expr>(identifier_name, std::move(expr));
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_unary_expr() {
    // TODO: rethink this
    eat();
    switch (current_token_.type) {
        case lexer::token_type::plus:
        case lexer::token_type::minus:
        case lexer::token_type::bitwise_not:
        case lexer::token_type::exclamation_mark:
        case lexer::token_type::plus_plus:
        case lexer::token_type::minus_minus:
            return parse_prefix_expr();
        default:
            putback_tokens_.push(current_token_);
            break;
    }

    return parse_postfix_expr();
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_prefix_expr() {
    auto prefix_token_type = current_token_.type;
    auto primary_expr = parse_primary_expr();

    return std::make_unique<ast::prefix_expr>(
            std::move(primary_expr), token_to_unary_operation[prefix_token_type]);
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_postfix_expr() {
    auto primary_expr = parse_primary_expr();

    eat();
    switch (current_token_.type) {
        case lexer::token_type::plus_plus:
        case lexer::token_type::minus_minus:
            return std::make_unique<ast::postfix_expr>(
                    std::move(primary_expr), token_to_unary_operation[current_token_.type]);
        default:
            putback_tokens_.push(current_token_);
            break;
    }

    return primary_expr;
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

        auto right = parse_unary_expr();

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

std::unique_ptr<parser::ast::expr> parser::parser::parse_primary_expr() {
    eat();
    auto priv_token = current_token_;
    switch (current_token_.type) {
        case lexer::token_type::number: return parse_integer_expr();
        case lexer::token_type::boolean: return parse_boolean_expr();
        case lexer::token_type::identifier:
            eat();
            if (is_current_token_assignment_token()) {
                putback_tokens_.push(current_token_);
                putback_tokens_.push(priv_token);
                return parse_assignment_expr();
            }

            putback_tokens_.push(current_token_);
            current_token_ = priv_token;
            return parse_identifier_expr();
        case lexer::token_type::l_parenthesis:
            putback_tokens_.push(current_token_);
            return parse_parenthesis_expr();
        default:
            unexpected_token_error();
            __builtin_unreachable();
    }
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_parenthesis_expr() {
    eat();
    expect(lexer::token_type::l_parenthesis);

    auto expr = parse_expr();

    eat();
    expect(lexer::token_type::r_parenthesis);

    return std::move(expr);
}

std::unique_ptr<parser::ast::call_expr> parser::parser::parse_call_expr() {
    eat();
    expect(lexer::token_type::identifier);
    auto identifier = current_token_.value;

    auto arguments = parse_call_arguments();

    return std::make_unique<ast::call_expr>(identifier, std::move(arguments));
}

std::vector<std::unique_ptr<parser::ast::expr>> parser::parser::parse_call_arguments() {
    std::vector<std::unique_ptr<ast::expr>> arguments;

    eat();
    expect(lexer::token_type::l_parenthesis);

    eat();
    do {
        if (current_token_.type == lexer::token_type::r_parenthesis) {
            break;
        }

        putback_tokens_.push(current_token_);
        arguments.push_back(parse_expr());

        eat();
        if (current_token_.type == lexer::token_type::coma) {
            eat();
        }
    } while (has_tokens());

    return arguments;
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_identifier_expr() {
    auto identifier_token = current_token_;
    auto identifier = identifier_token.value;

    eat();
    if (current_token_.type == lexer::token_type::l_parenthesis) {
        putback_tokens_.push(current_token_);
        putback_tokens_.push(identifier_token);
        return parse_call_expr();
    }
    putback_tokens_.push(current_token_);

    return std::make_unique<ast::identifier_expr>(identifier);
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
