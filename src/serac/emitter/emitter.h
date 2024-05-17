#ifndef SERALANG_EMITTER_H
#define SERALANG_EMITTER_H

#include <memory>
#include <algorithm>
#include <map>
#include <unordered_set>

#include "../utils/log_error.h"
#include "../parser/ast/translation_ast.h"
#include "../parser/ast/stmts/let_stmt.h"
#include "../parser/ast/stmts/return_stmt.h"
#include "../parser/ast/stmts/func_decl_stmt.h"
#include "../parser/ast/exprs/binary_expr.h"
#include "../parser/ast/exprs/call_expr.h"
#include "../parser/ast/exprs/identifier_expr.h"
#include "ir/package_ir.h"
#include "ir/types/func_type.h"
#include "ir/exprs/binary_expr_ir.h"
#include "ir/exprs/arithmetic_expr_ir.h"
#include "ir/exprs/logical_expr_ir.h"
#include "ir/exprs/relational_expr_ir.h"
#include "ir/exprs/bitwise_expr_ir.h"
#include "ir/variable_ir.h"
#include "ir/stmts/return_stmt_ir.h"
#include "ir/exprs/identifier_expr_ir.h"
#include "ir/exprs/argument_expr_ir.h"
#include "ir/exprs/call_expr_ir.h"
#include "ir/exprs/integer_expr_ir.h"
#include "ir/exprs/boolean_expr_ir.h"
#include "ir/exprs/cast_expr_ir.h"
#include "ir/exprs/upcast_expr_ir.h"
#include "ir/exprs/downcast_expr_ir.h"
#include "../parser/ast/stmts/if_stmt.h"
#include "ir/stmts/else_if_stmt_ir.h"
#include "ir/stmts/else_stmt_ir.h"
#include "ir/stmts/if_stmt_ir.h"
#include "ir/stmts/do_while_stmt_ir.h"
#include "ir/stmts/whille_stmt_ir.h"
#include "../parser/ast/stmts/while_stmt.h"
#include "../parser/ast/stmts/do_while_stmt.h"
#include "ir/stmts/break_stmt_ir.h"
#include "ir/stmts/breakall_stmt_ir.h"
#include "../parser/ast/stmts/break_stmt.h"
#include "../parser/ast/stmts/breakall_stmt.h"
#include "../parser/ast/stmts/continue_stmt.h"
#include "ir/stmts/continue_stmt_ir.h"
#include "../parser/ast/stmts/loop_stmt.h"
#include "ir/stmts/loop_stmt_ir.h"
#include "../parser/ast/stmts/for_stmt.h"
#include "../parser/ast/exprs/unary_expr.h"
#include "ir/exprs/unary_expr_ir.h"
#include "ir/exprs/prefix_expr_ir.h"
#include "../parser/ast/exprs/prefix_expr.h"
#include "ir/exprs/postfix_expr_ir.h"
#include "../parser/ast/exprs/postfix_expr.h"
#include "ir/types/unary_operation_type.h"
#include "../parser/ast/exprs/assignment_expr.h"
#include "ir/exprs/assignment_expr_ir.h"
#include "ir/stmts/expr_stmt_ir.h"
#include "../parser/ast/stmts/expr_stmt.h"

namespace emitter {

    class emitter {
    public:
        explicit emitter(std::vector<std::unique_ptr<parser::ast::translation_ast>> translation_asts) :
            translation_asts_(std::move(translation_asts)) {}

            std::unique_ptr<ir::package_ir> emit();

    private:
        std::vector<std::unique_ptr<parser::ast::translation_ast>> translation_asts_;

        std::vector<std::unique_ptr<ir::variable_ir>> global_variables_;
        std::vector<std::unique_ptr<ir::func_decl_ir>> functions_;

        std::unordered_set<std::string> declared_functions_;
        std::unordered_set<std::string> declared_global_variables_;

        std::map<std::string, ir::func_type*> functions_types_;
        std::map<std::string, ir::type*> global_variables_types_;

        std::map<std::string, ir::type*> types_ = {
                {"bool", ir::type::boolean()},
                {"int1", ir::integer_type::int1()},
                {"int8", ir::integer_type::int8()},
                {"int16", ir::integer_type::int16()},
                {"int32", ir::integer_type::int32()},
                {"int", ir::integer_type::int32()},
                {"int64", ir::integer_type::int64()},
                {"uint8", ir::integer_type::uint8()},
                {"uint16", ir::integer_type::uint16()},
                {"uint32", ir::integer_type::uint32()},
                {"uint", ir::integer_type::uint32()},
                {"uint64", ir::integer_type::uint64()}
        };

        ir::scope_stmt_ir* current_scope_ = nullptr;
        ir::func_type* current_function_ = nullptr;

        int loop_count_ = -1;
        bool skip_all_statements_forward_ = false;

        void find_globals();

        bool is_identifier_is_func_argument(const std::string& name);
        std::tuple<ir::type*, int> get_type_for_func_argument(const std::string& name);

        std::string generate_func_static_var_name(const std::string& var_name);

        void emit_all_global_variables();
        void emit_all_func_declarations();
        std::unique_ptr<ir::func_decl_ir> emit_for_func(parser::ast::func_decl_stmt* func_stmt);
        std::vector<ir::func_param_ir> emit_func_params(parser::ast::func_decl_stmt* func_stmt);
        ir::func_param_ir emit_for_func_param(const parser::ast::func_param& func_param);

        void emit_for_stmt(std::unique_ptr<parser::ast::stmt> stmt);
        std::unique_ptr<ir::scope_stmt_ir> emit_for_scope_stmt(parser::ast::scope_stmt* scope_stmt);
        void emit_for_if_stmt(parser::ast::if_stmt* if_stmt);
        std::vector<std::unique_ptr<ir::else_if_stmt_ir>>
        emit_for_else_if_stmts(std::vector<std::unique_ptr<parser::ast::else_if_stmt>> else_if_branches);
        std::unique_ptr<ir::else_if_stmt_ir> emit_for_else_if_stmt(parser::ast::else_if_stmt* else_if_stmt);
        std::unique_ptr<ir::else_stmt_ir> emit_for_else_stmt(parser::ast::else_stmt* else_stmt);
        void emit_for_while_stmt(parser::ast::while_stmt* while_stmt);
        void emit_for_do_while_stmt(parser::ast::do_while_stmt* do_while_stmt);
        void emit_for_loop_stmt(parser::ast::loop_stmt* loop_stmt);
        void emit_for_for_stmt(parser::ast::for_stmt* for_stmt);
        void emit_for_let_stmt(parser::ast::let_stmt* let_stmt);
        void emit_for_expr_stmt(parser::ast::expr_stmt* expr_stmt);
        void emit_for_return_stmt(parser::ast::return_stmt* return_stmt);
        void emit_for_break_stmt(parser::ast::break_stmt* break_stmt);
        void emit_for_breakall_stmt();
        void emit_for_continue_stmt();

        std::unique_ptr<ir::expr_ir> emit_for_expr(std::unique_ptr<parser::ast::expr> expr);

        std::unique_ptr<ir::assignment_expr_ir>
        emit_for_assignment_expr(parser::ast::assignment_expr* assignment_expr);

        std::unique_ptr<ir::unary_expr_ir> emit_for_unary_expr(parser::ast::unary_expr* unary_expr);
        std::unique_ptr<ir::prefix_expr_ir> emit_for_prefix_expr(parser::ast::prefix_expr* prefix_expr);
        std::unique_ptr<ir::postfix_expr_ir> emit_for_postfix_expr(parser::ast::postfix_expr* postfix_expr);

        std::unique_ptr<ir::binary_expr_ir> emit_for_binary_expr(parser::ast::binary_expr* binary_expr);

        static std::unique_ptr<ir::expr_ir> emit_for_cast(std::unique_ptr<ir::expr_ir> expr, ir::type* cast_to);
        static std::tuple<std::unique_ptr<ir::expr_ir>, std::unique_ptr<ir::expr_ir>, ir::type*>
        emit_for_cast(std::unique_ptr<ir::expr_ir> left, std::unique_ptr<ir::expr_ir> right);
        static std::unique_ptr<ir::upcast_expr_ir>
        emit_for_upcast(std::unique_ptr<ir::expr_ir> inner_expr, ir::type* cast_to_type);
        static std::unique_ptr<ir::downcast_expr_ir>
        emit_for_downcast(std::unique_ptr<ir::expr_ir> inner_expr, ir::type* cast_to_type);

        std::unique_ptr<ir::identifier_expr_ir> emit_for_identifier_expr(parser::ast::identifier_expr* identifier_expr);
        std::unique_ptr<ir::call_expr_ir> emit_for_call_expr(parser::ast::call_expr* call_expr);
        static std::unique_ptr<ir::boolean_expr_ir> emit_for_boolean_expr(parser::ast::boolean_expr* boolean_expr);
        static std::unique_ptr<ir::integer_expr_ir> emit_for_integer_expr(parser::ast::integer_expr* integer_expr);
        static std::unique_ptr<ir::integer_expr_ir>
        emit_for_explicitly_typed_integer(parser::ast::integer_expr* integer_expr, const std::string& explicit_int_type);
    };

}

#endif //SERALANG_EMITTER_H
