#ifndef SNAKELANG_TRANSLATOR_H
#define SNAKELANG_TRANSLATOR_H

#include <utility>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <map>

#include "../emitter/ir/package_ir.h"
#include "../emitter/ir/exprs/integer_expr_ir.h"
#include "../emitter/ir/exprs/boolean_expr_ir.h"
#include "../emitter/ir/exprs/binary_expr_ir.h"

#include "../utils/log_error.h"
#include "../emitter/ir/stmts/return_stmt_ir.h"
#include "../emitter/ir/exprs/call_expr_ir.h"
#include "../emitter/ir/stmts/call_stmt_ir.h"
#include "../emitter/ir/exprs/identifier_expr_ir.h"
#include "../emitter/ir/exprs/argument_expr_ir.h"
#include "../emitter/ir/exprs/cast_expr_ir.h"
#include "../emitter/ir/exprs/upcast_expr_ir.h"
#include "../emitter/ir/exprs/downcast_expr_ir.h"
#include "../emitter/ir/stmts/assignment_stmt_ir.h"
#include "../emitter/ir/exprs/arithmetic_expr_ir.h"
#include "../emitter/ir/exprs/relational_expr_ir.h"
#include "../emitter/ir/exprs/logical_expr_ir.h"
#include "../emitter/ir/exprs/bitwise_expr_ir.h"
#include "../emitter/ir/stmts/if_stmt_ir.h"
#include "../emitter/ir/stmts/whille_stmt_ir.h"
#include "../emitter/ir/stmts/do_while_stmt_ir.h"
#include "../emitter/ir/stmts/break_stmt_ir.h"
#include "../emitter/ir/stmts/breakall_stmt_ir.h"
#include "../emitter/ir/stmts/continue_stmt_ir.h"
#include "../emitter/ir/stmts/loop_stmt_ir.h"

namespace translator {

    class translator {
    public:
        explicit translator(std::unique_ptr<emitter::ir::package_ir> package_ir) :
            package_ir_(std::move(package_ir)),
            context_(std::make_unique<llvm::LLVMContext>()),
            module_(std::make_unique<llvm::Module>(package_ir_->name, *context_)),
            builder_(std::make_unique<llvm::IRBuilder<>>(*context_)) {}

        std::unique_ptr<llvm::Module> translate();

    private:
        std::unique_ptr<emitter::ir::package_ir> package_ir_;

        std::unique_ptr<llvm::LLVMContext> context_;
        std::unique_ptr<llvm::Module> module_;
        std::unique_ptr<llvm::IRBuilder<>> builder_;

        std::map<std::string, llvm::Type*> types_;

        llvm::BasicBlock* current_allocation_block_ = nullptr;
        llvm::BasicBlock* current_block_ = nullptr;
        llvm::BasicBlock* priv_block_ = nullptr;
        llvm::BasicBlock* insert_before_block_ = nullptr;

        llvm::BasicBlock* break_to_block_ = nullptr;
        std::vector<llvm::BasicBlock*> break_to_blocks_;
        llvm::BasicBlock* breakall_to_block_ = nullptr;
        llvm::BasicBlock* continue_to_block_ = nullptr;

        bool br_generated_ = false;
        int inner_loops_ = -1;

        llvm::Function* current_function_ = nullptr;

        emitter::ir::scope_stmt_ir* current_scope_ = nullptr;
        std::map<std::string, llvm::Value*> local_variables_;

        void create_types();
        void create_basic_types();

        void declare_functions();

        void translate_global_vars();
        void translate_global_var(std::unique_ptr<emitter::ir::variable_ir> variable_ir, bool &generate_br);

        void translate_function_declarations();
        void translate_main_function(std::unique_ptr<emitter::ir::func_decl_ir> main_decl_ir);
        llvm::Function* translate_function(std::unique_ptr<emitter::ir::func_decl_ir> func_decl_ir);

        void translate_stmt(std::unique_ptr<emitter::ir::stmt_ir> stmt_ir);
        void translate_scope_stmt(emitter::ir::scope_stmt_ir* scope_ir);
        void translate_if_stmt(emitter::ir::if_stmt_ir* if_stmt);
        void translate_while_stmt(emitter::ir::while_stmt_ir* while_stmt);
        void translate_do_while_stmt(emitter::ir::do_while_stmt_ir* do_while_stmt);
        void translate_loop_stmt(emitter::ir::loop_stmt_ir* loop_stmt);
        void translate_var_stmt(emitter::ir::variable_ir* variable_ir);
        void translate_assignment_stmt(emitter::ir::assignment_stmt_ir* assignment_stmt);
        void translate_call_stmt(emitter::ir::call_stmt_ir* call_stmt);
        void translate_return_stmt(emitter::ir::return_stmt_ir* return_ir);
        void translate_break_stmt(emitter::ir::break_stmt_ir* break_stmt);
        void translate_breakall_stmt();
        void translate_continue_stmt();

        llvm::Value* translate_expr(emitter::ir::expr_ir* expr);
        llvm::Constant* translate_int_expr(emitter::ir::integer_expr_ir* integer_expr);
        llvm::Constant* translate_boolean_expr(emitter::ir::boolean_expr_ir* boolean_expr);
        llvm::Value* translate_binary_expr(emitter::ir::binary_expr_ir* binary_expr);
        llvm::Value* translate_arithmetic_expr(emitter::ir::arithmetic_expr_ir* arithmetic_expr);
        llvm::Value* translate_relational_expr(emitter::ir::relational_expr_ir* relational_expr);
        llvm::Value* translate_logical_expr(emitter::ir::logical_expr_ir* logical_expr);
        llvm::Value* translate_bitwise_expr(emitter::ir::bitwise_expr_ir* bitwise_expr);
        llvm::Value* translate_call_expr(emitter::ir::call_expr_ir* call_expr);
        llvm::Value* translate_identifier_expr(emitter::ir::identifier_expr_ir* identifier_expr);
        llvm::Value* translate_argument_expr(emitter::ir::argument_exp_ir* argument_expr);
        llvm::Value* translate_cast_expr(emitter::ir::cast_expr_ir* cast_expr);
        llvm::Value* translate_upcast_expr(emitter::ir::upcast_expr_ir* upcast_expr);
        llvm::Value* translate_downcast_expr(emitter::ir::downcast_expr_ir* downcast_expr);
    };

}

#endif //SNAKELANG_TRANSLATOR_H
