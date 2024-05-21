#include "parser.h"

std::unique_ptr<parser::ast::translation_ast> parser::parser::parse() {
    std::vector<std::unique_ptr<ast::top_stmt>> stmts;

    auto package_stmt = parse_package_stmt();
    if (!package_stmt) {
        return nullptr;
    }

    eat();
    while (has_tokens()) {
        auto top_stmt = parse_top_stmt();
        if (!top_stmt) {
            return nullptr;
        }

        stmts.push_back(std::move(top_stmt));
        eat();
    }

    return std::make_unique<ast::translation_ast>(std::move(package_stmt), std::move(stmts));
}

bool parser::parser::has_tokens() const {
    return current_token_.type != lexer::token_type::eof;
}

bool parser::parser::expect(lexer::token_type token_type) {
    if (current_token_.type != token_type) {
        auto expected_token_type = lexer::convert_token_type_to_string(token_type);
        auto got_token_type = lexer::convert_token_type_to_string(current_token_.type);

        error = std::make_unique<errors::error>(
                std::format(
                        R"(Expected: "{}", but got: "{}".)",
                        expected_token_type,
                        got_token_type),
                std::format(
                        R"(try add/replace "{}" to "{}".)",
                        got_token_type,
                        expected_token_type
                        ),
                file_name_,
                current_token_.line,
                current_token_.column_start,
                current_token_.column_end);

        return false;
    }

    return true;
}

bool parser::parser::expect(std::vector<lexer::token_type> token_types) {
    std::stringstream expected_tokens;
    for (auto token_type : token_types) {
        if (current_token_.type == token_type) {
            return true;
        }

        auto place_coma = *token_types.end().base() != token_type;

        expected_tokens
            << '"'
            << lexer::convert_token_type_to_string(token_type)
            << '"'
            << (place_coma ? ", " : "");
    }

    auto got_token_type = lexer::convert_token_type_to_string(current_token_.type);

    error = std::make_unique<errors::error>(
            std::format(
                    "Unexpected token: \"{}\", expected one of these tokens: ({}).",
                    got_token_type,
                    expected_tokens.str()),
            std::format(
                    "try add/replace \"{}\" to one of these: ({}).",
                    got_token_type,
                    expected_tokens.str()
            ),
            file_name_,
            current_token_.line,
            current_token_.column_start,
            current_token_.column_end);

    return false;
}

void parser::parser::unexpected() {
    error = std::make_unique<errors::error>(
            std::format(
                    "Unexpected token: \"{}\" found.",
                    lexer::convert_token_type_to_string(current_token_.type)),
            "try to remove this token.",
            file_name_,
            current_token_.line,
            current_token_.column_start,
            current_token_.column_end);
}

void parser::parser::eat() {
    if (!putback_tokens_.empty()) {
        current_token_ = putback_tokens_.top();
        putback_tokens_.pop();
        return;
    }

    current_token_ = lexer_.get_next_token();
}

void parser::parser::putback() {
    putback_tokens_.push(current_token_);
}

void parser::parser::putback(const lexer::token &token) {
    putback_tokens_.push(token);
}

void parser::parser::store_priv_token() {
    priv_token_ = current_token_;
}

parser::ast::file_metadata
parser::parser::create_metadata(const lexer::token &first_token, const lexer::token& last_token) {
    return ast::file_metadata(
            file_name_,
            first_token.line,
            last_token.line,
            first_token.column_start,
            last_token.column_end);
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
    if (!expect(lexer::token_type::package)) {
        return nullptr;
    }
    auto first_token = current_token_;

    std::string package_name;

    eat();
    if (!expect(lexer::token_type::identifier)) {
        return nullptr;
    }
    package_name += current_token_.value;

    eat();
    if (current_token_.type == lexer::token_type::semicolon) {
        auto package_stmt = std::make_unique<ast::package_stmt>(package_name);
        package_stmt->metadata = create_metadata(first_token, current_token_);
        return package_stmt;
    }

    while (has_tokens()) {
        if (!expect(lexer::token_type::dot)) {
            return nullptr;
        }
        package_name += current_token_.value;

        eat();
        if (!expect(lexer::token_type::identifier)) {
            return nullptr;
        }
        package_name += current_token_.value;

        eat();
        if (current_token_.type == lexer::token_type::semicolon) {
            break;
        }
    }

    auto package_stmt = std::make_unique<ast::package_stmt>(package_name);
    package_stmt->metadata = create_metadata(first_token, current_token_);
    return package_stmt;
}

std::unique_ptr<parser::ast::top_stmt> parser::parser::parse_top_stmt() {
    std::unique_ptr<ast::top_stmt> top_stmt;

    auto first_token = current_token_;
    switch (current_token_.type) {
        case lexer::token_type::static_:
            eat();
            top_stmt = parse_let_stmt(true);
            break;
        case lexer::token_type::let:
            top_stmt = parse_let_stmt(false);
            break;
        case lexer::token_type::func:
            top_stmt = parse_func_decl_stmt();
            break;
        default:
            unexpected();
            return nullptr;
    }

    if (!top_stmt) {
        return nullptr;
    }

    top_stmt->metadata = create_metadata(first_token, current_token_);
    return top_stmt;
}

std::unique_ptr<parser::ast::func_decl_stmt> parser::parser::parse_func_decl_stmt() {
    if (!expect(lexer::token_type::func)) {
        return nullptr;
    }

    eat();
    if (!expect(lexer::token_type::identifier)) {
        return nullptr;
    }

    auto function_name = current_token_.value;

    auto params = parse_func_params();

    eat();
    if (!expect(lexer::token_type::identifier)) {
        return nullptr;
    }
    auto return_type = current_token_.value;

    eat();
    auto first_scope_token = current_token_;
    putback(first_scope_token);

    auto scope_stmt = parse_scope_stmt();
    if (!scope_stmt) {
        return nullptr;
    }

    scope_stmt->metadata = create_metadata(first_scope_token, current_token_);

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


        if (!expect(lexer::token_type::identifier)) {
            return params;
        }
        auto param_name = current_token_.value;

        eat();
        if (!expect(lexer::token_type::colon)) {
            return params;
        }

        eat();
        if (!expect(lexer::token_type::identifier)) {
            return params;
        }
        auto param_type = current_token_.value;

        params.emplace_back(param_name, param_type);

        eat();
        if (current_token_.type == lexer::token_type::coma) {
            eat();
        }
    } while (has_tokens());

    return params;
}

std::unique_ptr<parser::ast::stmt> parser::parser::parse_stmt() {
    std::unique_ptr<ast::stmt> stmt;

    eat();
    auto first_token = current_token_;
    switch (current_token_.type) {
        case lexer::token_type::if_:
        case lexer::token_type::while_:
        case lexer::token_type::do_:
        case lexer::token_type::loop:
        case lexer::token_type::for_:
            stmt = parse_compound_stmt();
            break;
        case lexer::token_type::break_:
        case lexer::token_type::breakall:
        case lexer::token_type::continue_:
        case lexer::token_type::ret:
            stmt = parse_control_flow_stmt();
            break;
        case lexer::token_type::l_curly_brace:
            putback();
            stmt = parse_scope_stmt();
            break;
        default:
            putback();
            stmt = parse_local_stmt();
    }

    if (!stmt) {
        return nullptr;
    }

    stmt->metadata = create_metadata(first_token, current_token_);
    return stmt;
}

std::unique_ptr<parser::ast::scope_stmt> parser::parser::parse_scope_stmt() {
    eat();
    if (!expect(lexer::token_type::l_curly_brace)) {
        return nullptr;
    }

    std::vector<std::unique_ptr<ast::stmt>> inner_stmts;

    eat();
    while (has_tokens() && current_token_.type != lexer::token_type::r_curly_brace) {
        putback();

        auto stmt = parse_stmt();
        if (!stmt) {
            return nullptr;
        }

        inner_stmts.push_back(std::move(stmt));
        eat();
    }

    if (!expect(lexer::token_type::r_curly_brace)) {
        return nullptr;
    }

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
            return nullptr;
    }
}

std::unique_ptr<parser::ast::if_stmt> parser::parser::parse_if_stmt() {
    if (!expect(lexer::token_type::if_)) {
        return nullptr;
    }

    auto if_expr = parse_parenthesis_expr();
    if (!if_expr) {
        return nullptr;
    }

    auto if_scope = parse_scope_stmt();
    if (!if_scope) {
        return nullptr;
    }

    std::vector<std::unique_ptr<ast::else_if_stmt>> else_if_branches;
    std::unique_ptr<ast::else_stmt> else_branch;

    eat();
    auto else_token = current_token_;
    if (current_token_.type != lexer::token_type::else_) {
        putback(else_token);

        return std::make_unique<ast::if_stmt>(
                std::move(if_expr),
                std::move(if_scope),
                std::move(else_if_branches),
                std::move(else_branch));
    }

    eat();
    if (current_token_.type == lexer::token_type::if_) {
        putback();
        putback(else_token);

        else_if_branches = parse_else_if_stmts();
    } else {
        putback();
        putback(else_token);
    }

    eat();
    if (current_token_.type != lexer::token_type::else_) {
        putback();

        return std::make_unique<ast::if_stmt>(
                std::move(if_expr),
                std::move(if_scope),
                std::move(else_if_branches),
                std::move(else_branch));
    }

    putback();
    else_branch = parse_else_stmt();
    if (!else_branch) {
        return nullptr;
    }

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
            putback();
            current_token_ = else_token;
            break;
        }

        auto else_if_stmt = parse_else_if_stmt();
        if (!else_if_stmt) {
            return else_if_branches;
        }

        else_if_branches.push_back(std::move(else_if_stmt));
        eat();
    }

    putback();

    return else_if_branches;
}

std::unique_ptr<parser::ast::else_if_stmt> parser::parser::parse_else_if_stmt() {
    if (!expect(lexer::token_type::if_)) {
        return nullptr;
    }

    auto else_if_expr = parse_parenthesis_expr();
    if (!else_if_expr) {
        return nullptr;
    }

    auto else_if_scope = parse_scope_stmt();
    if (!else_if_scope) {
        return nullptr;
    }

    return std::make_unique<ast::else_if_stmt>(std::move(else_if_expr), std::move(else_if_scope));
}

std::unique_ptr<parser::ast::else_stmt> parser::parser::parse_else_stmt() {
    eat();
    if (!expect(lexer::token_type::else_)) {
        return nullptr;
    }

    auto else_scope = parse_scope_stmt();
    if (!else_scope) {
        return nullptr;
    }

    return std::make_unique<ast::else_stmt>(std::move(else_scope));
}

std::unique_ptr<parser::ast::while_stmt> parser::parser::parse_while_stmt() {
    if (!expect(lexer::token_type::while_)) {
        return nullptr;
    }

    auto condition_expr = parse_parenthesis_expr();
    if (!condition_expr) {
        return nullptr;
    }

    auto while_scope = parse_scope_stmt();
    if (!while_scope) {
        return nullptr;
    }

    return std::make_unique<ast::while_stmt>(std::move(condition_expr), std::move(while_scope));
}

std::unique_ptr<parser::ast::do_while_stmt> parser::parser::parse_do_while_stmt() {
    if (!expect(lexer::token_type::do_)) {
        return nullptr;
    }

    auto do_while_scope = parse_scope_stmt();
    if (!do_while_scope) {
        return nullptr;
    }

    eat();
    if (!expect(lexer::token_type::while_)) {
        return nullptr;
    }

    auto condition_expr = parse_parenthesis_expr();
    if (!condition_expr) {
        return nullptr;
    }

    eat();
    if (!expect(lexer::token_type::semicolon)) {
        return nullptr;
    }

    return std::make_unique<ast::do_while_stmt>(std::move(condition_expr), std::move(do_while_scope));
}

std::unique_ptr<parser::ast::loop_stmt> parser::parser::parse_loop_stmt() {
    if (!expect(lexer::token_type::loop)) {
        return nullptr;
    }

    auto scope = parse_scope_stmt();
    if (!scope) {
        return nullptr;
    }

    return std::make_unique<ast::loop_stmt>(std::move(scope));
}

std::unique_ptr<parser::ast::for_stmt> parser::parser::parse_for_stmt() {
    if (!expect(lexer::token_type::for_)) {
        return nullptr;
    }

    eat();
    if (!expect(lexer::token_type::l_parenthesis)) {
        return nullptr;
    }

    std::vector<std::unique_ptr<ast::stmt>> run_once;

    eat();
    if (current_token_.type != lexer::token_type::semicolon) {
        putback();
    }

    while (current_token_.type != lexer::token_type::semicolon) {
        eat();
        auto first_token = current_token_;
        putback();

        auto stmt = parse_local_stmt(false);
        if (!stmt) {
            return nullptr;
        }
        run_once.push_back(std::move(stmt));

        if (current_token_.type != lexer::token_type::coma
            && current_token_.type != lexer::token_type::semicolon) {
            if (!expect(lexer::token_type::coma)) {
                return nullptr;
            }
        }
    }

    auto condition = parse_expr();
    if (!condition) {
        return nullptr;
    }

    eat();
    if (!expect(lexer::token_type::semicolon)) {
        return nullptr;
    }

    std::vector<std::unique_ptr<ast::expr>> run_after_each;

    eat();
    if (current_token_.type != lexer::token_type::r_parenthesis) {
        putback();
    }

    while (current_token_.type != lexer::token_type::r_parenthesis) {
        auto stmt = parse_expr();
        if (!stmt) {
            return nullptr;
        }
        run_after_each.push_back(std::move(stmt));
        eat();

        if (current_token_.type != lexer::token_type::coma
            && current_token_.type != lexer::token_type::r_parenthesis) {
            if (!expect(lexer::token_type::coma)) {
                return nullptr;
            }
        }
    }

    auto scope = parse_scope_stmt();
    if (!scope) {
        return nullptr;
    }

    return std::make_unique<ast::for_stmt>(
            std::move(run_once), std::move(condition), std::move(run_after_each), std::move(scope));
}

std::unique_ptr<parser::ast::local_stmt> parser::parser::parse_local_stmt(bool expect_semicolon) {
    std::unique_ptr<ast::local_stmt> local_stmt;

    eat();
    auto first_token = current_token_;
    switch (current_token_.type) {
        case lexer::token_type::let:
            local_stmt = parse_let_stmt(false, expect_semicolon);
            break;
        case lexer::token_type::static_:
            eat();
            local_stmt = parse_let_stmt(true, expect_semicolon);
            break;
        default:
            local_stmt = parse_expr_stmt(expect_semicolon);
    }

    if (!local_stmt) {
        return nullptr;
    }

    local_stmt->metadata = create_metadata(first_token, current_token_);
    return local_stmt;
}

std::unique_ptr<parser::ast::let_stmt> parser::parser::parse_let_stmt(bool is_static, bool expect_semicolon) {
    if (!expect(lexer::token_type::let)) {
        return nullptr;
    }

    eat();
    if (!expect(lexer::token_type::identifier)) {
        return nullptr;
    }
    auto identifier_name = current_token_.value;

    eat();
    if (!expect(lexer::token_type::assign)) {
        return nullptr;
    }

    auto expression = parse_expr();
    if (!expression) {
        return nullptr;
    }

    eat();
    if (expect_semicolon && !expect(lexer::token_type::semicolon)) {
        return nullptr;
    }

    return std::make_unique<ast::let_stmt>(identifier_name, std::move(expression), is_static);
}

std::unique_ptr<parser::ast::expr_stmt> parser::parser::parse_expr_stmt(bool expect_semicolon) {
    putback();
    auto expr = parse_expr();
    if (!expr) {
        return nullptr;
    }

    eat();
    if (expect_semicolon && !expect(lexer::token_type::semicolon)) {
        return nullptr;
    }

    return std::make_unique<ast::expr_stmt>(std::move(expr));
}

std::unique_ptr<parser::ast::control_flow_stmt> parser::parser::parse_control_flow_stmt() {
    std::unique_ptr<ast::control_flow_stmt> control_flow_stmt;

    auto first_token = current_token_;
    switch (current_token_.type) {
        case lexer::token_type::break_:
            control_flow_stmt = parse_break_stmt();
            break;
        case lexer::token_type::breakall:
            control_flow_stmt = parse_breakall_stmt();
            break;
        case lexer::token_type::continue_:
            control_flow_stmt = parse_continue_stmt();
            break;
        case lexer::token_type::ret:
            control_flow_stmt = parse_return_stmt();
            break;
        default:
            return nullptr;
    }

    if (!control_flow_stmt) {
        return nullptr;
    }

    control_flow_stmt->metadata = create_metadata(first_token, current_token_);
    return control_flow_stmt;
}

std::unique_ptr<parser::ast::return_stmt> parser::parser::parse_return_stmt() {
    if (!expect(lexer::token_type::ret)) {
        return nullptr;
    }
    
    auto expr = parse_expr();
    if (!expr) {
        return nullptr;
    }
    
    auto return_stmt = std::make_unique<ast::return_stmt>(std::move(expr));

    eat();
    if (!expect(lexer::token_type::semicolon)) {
        return nullptr;
    }

    return std::move(return_stmt);
}

std::unique_ptr<parser::ast::break_stmt> parser::parser::parse_break_stmt() {
    if (!expect(lexer::token_type::break_)) {
        return nullptr;
    }

    eat();
    if (current_token_.type == lexer::token_type::semicolon) {
        return std::make_unique<ast::break_stmt>();
    }

    putback();
    auto break_expr = parse_expr();
    if (!break_expr) {
        return nullptr;
    }

    eat();
    if (!expect(lexer::token_type::semicolon)) {
        return nullptr;
    }

    return std::make_unique<ast::break_stmt>(std::move(break_expr));
}

std::unique_ptr<parser::ast::breakall_stmt> parser::parser::parse_breakall_stmt() {
    if (!expect(lexer::token_type::breakall)) {
        return nullptr;
    }

    eat();
    if (!expect(lexer::token_type::semicolon)) {
        return nullptr;
    }

    return std::make_unique<ast::breakall_stmt>();
}

std::unique_ptr<parser::ast::continue_stmt> parser::parser::parse_continue_stmt() {
    if (!expect(lexer::token_type::continue_)) {
        return nullptr;
    }

    eat();
    if (!expect(lexer::token_type::semicolon)) {
        return nullptr;
    }

    return std::make_unique<ast::continue_stmt>();
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_expr() {
    eat();
    auto first_token = current_token_;
    putback();
    store_priv_token();
    auto left = parse_unary_expr();
    if (!left) {
        return nullptr;
    }

    eat();
    if (current_token_.type == lexer::token_type::cast) {
        left = parse_cast_expr(std::move(left));
        if (!left) {
            return nullptr;
        }
    }
    putback();

    auto expr = parse_binary_expr(std::move(left), 0);
    if (!expr) {
        return nullptr;
    }
    
    expr->metadata = create_metadata(first_token, priv_token_);
    return expr;
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_cast_expr(std::unique_ptr<ast::expr> expr) {
    if (!expect(lexer::token_type::cast)) {
        return nullptr;
    }

    eat();
    if (current_token_.type != lexer::token_type::identifier) {
        return parse_complex_cast_expr(std::move(expr));
    }

    auto cast_expr = std::make_unique<ast::cast_expr>(
            std::move(expr), current_token_.value);
    cast_expr->metadata = create_metadata(priv_token_, current_token_);

    return cast_expr;
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_complex_cast_expr(std::unique_ptr<ast::expr> expr) {
    if (!expect(lexer::token_type::l_curly_brace)) {
        return nullptr;
    }

    eat();
    if (!expect(lexer::token_type::identifier)) {
        return nullptr;
    }
    auto new_identifier = current_token_.value;

    eat();
    if (!expect(lexer::token_type::colon)) {
        return nullptr;
    }

    eat();
    if (!expect(lexer::token_type::identifier)) {
        return nullptr;
    }
    auto cast_type = current_token_.value;

    eat();
    if (!expect(lexer::token_type::r_curly_brace)) {
        return nullptr;
    }

    return std::make_unique<ast::complex_cast_expr>(std::move(expr), cast_type, new_identifier);
}

std::unique_ptr<parser::ast::assignment_expr> parser::parser::parse_assignment_expr() {
    eat();
    if (!expect(lexer::token_type::identifier)) {
        return nullptr;
    }
    auto identifier_name = current_token_.value;

    eat();
    auto assign_token_type = current_token_.type;
    if (!expect({
        lexer::token_type::assign,
        lexer::token_type::plus_assign,
        lexer::token_type::minus_assign,
        lexer::token_type::multiply_assign,
        lexer::token_type::divide_assign,
        lexer::token_type::modulus_assign})) {
        return nullptr;
    }

    auto expr = parse_expr();
    if (!expr) {
        return nullptr;
    }

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
            putback();
            break;
    }

    return parse_postfix_expr();
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_prefix_expr() {
    auto prefix_token_type = current_token_.type;
    auto primary_expr = parse_primary_expr();
    if (!primary_expr) {
        return nullptr;
    }

    return std::make_unique<ast::prefix_expr>(
            std::move(primary_expr), token_to_unary_operation[prefix_token_type]);
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_postfix_expr() {
    auto primary_expr = parse_primary_expr();
    if (!primary_expr) {
        return nullptr;
    }

    store_priv_token();
    eat();
    switch (current_token_.type) {
        case lexer::token_type::plus_plus:
        case lexer::token_type::minus_minus:
            store_priv_token();
            return std::make_unique<ast::postfix_expr>(
                    std::move(primary_expr), token_to_unary_operation[current_token_.type]);
        default:
            putback();
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
            putback();
            return left;
        }

        auto binary_operator = current_token_type_to_binary_operation();

        auto right = parse_unary_expr();
        if (!right) {
            return nullptr;
        }

        eat();
        auto next_precedence = get_current_token_precedence();
        if (current_precedence < next_precedence) {
            putback();
            right = parse_binary_expr(std::move(right), current_precedence + 1);
            eat();
            if (!right) {
                return nullptr;
            }
        }

        left = std::make_unique<ast::binary_expr>(std::move(left), std::move(right), binary_operator);
    }
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_primary_expr() {
    eat();
    auto priv_token = current_token_;
    switch (current_token_.type) {
        case lexer::token_type::integer: return parse_integer_expr();
        case lexer::token_type::boolean: return parse_boolean_expr();
        case lexer::token_type::identifier:
            eat();
            switch (current_token_.type) {
                case lexer::token_type::assign:
                case lexer::token_type::plus_assign:
                case lexer::token_type::minus_assign:
                case lexer::token_type::multiply_assign:
                case lexer::token_type::divide_assign:
                case lexer::token_type::modulus_assign:
                    putback();
                    putback(priv_token);
                    return parse_assignment_expr();
                default:
                    break;
            }

            putback();
            current_token_ = priv_token;
            return parse_identifier_expr();
        case lexer::token_type::l_parenthesis:
            putback();
            return parse_parenthesis_expr();
        default:
            unexpected();
            return nullptr;
    }
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_parenthesis_expr() {
    eat();
    if (!expect(lexer::token_type::l_parenthesis)) {
        return nullptr;
    }

    auto expr = parse_expr();
    if (!expr) {
        return nullptr;
    }

    eat();
    if (!expect(lexer::token_type::r_parenthesis)) {
        return nullptr;
    }
    store_priv_token();

    return std::move(expr);
}

std::unique_ptr<parser::ast::call_expr> parser::parser::parse_call_expr() {
    eat();
    if (!expect(lexer::token_type::identifier)) {
        return nullptr;
    }
    auto identifier = current_token_.value;

    auto arguments = parse_call_arguments();
    store_priv_token();

    return std::make_unique<ast::call_expr>(identifier, std::move(arguments));
}

std::vector<std::unique_ptr<parser::ast::expr>> parser::parser::parse_call_arguments() {
    std::vector<std::unique_ptr<ast::expr>> arguments;

    eat();
    if (!expect(lexer::token_type::l_parenthesis)) {
        return arguments;
    }

    eat();
    do {
        if (current_token_.type == lexer::token_type::r_parenthesis) {
            break;
        }

        putback();
        auto expr = parse_expr();
        if (!expr) {
            return arguments;
        }

        arguments.push_back(std::move(expr));

        eat();
        if (current_token_.type == lexer::token_type::coma) {
            eat();
        }
    } while (has_tokens());

    return arguments;
}

std::unique_ptr<parser::ast::expr> parser::parser::parse_identifier_expr() {
    if (!expect(lexer::token_type::identifier)) {
        return nullptr;
    }

    auto identifier_token = current_token_;
    auto identifier = identifier_token.value;

    store_priv_token();
    eat();
    if (current_token_.type == lexer::token_type::l_parenthesis) {
        putback();
        putback(identifier_token);
        return parse_call_expr();
    }
    putback();

    return std::make_unique<ast::identifier_expr>(identifier);
}

std::unique_ptr<parser::ast::integer_expr> parser::parser::parse_integer_expr() {
    if (!expect(lexer::token_type::integer)) {
        return nullptr;
    }

    auto number = std::stoi(current_token_.value);

    store_priv_token();
    eat();
    if (current_token_.type == lexer::token_type::colon) {
        eat();
        if (!expect(lexer::token_type::identifier)) {
            return nullptr;
        }
        store_priv_token();

        return std::make_unique<ast::integer_expr>(number, current_token_.value);
    }
    putback();

    return std::make_unique<ast::integer_expr>(number);
}

std::unique_ptr<parser::ast::boolean_expr> parser::parser::parse_boolean_expr() {
    if (!expect(lexer::token_type::boolean)) {
        return nullptr;
    }

    store_priv_token();
    return std::make_unique<ast::boolean_expr>(current_token_.value == "true");
}
