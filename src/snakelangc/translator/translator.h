#ifndef SNAKELANG_TRANSLATOR_H
#define SNAKELANG_TRANSLATOR_H

#include <utility>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>
#include <map>

#include "../emitter/ir/package_ir.h"
#include "../emitter/ir/integer_expr_ir.h"
#include "../emitter/ir/boolean_expr_ir.h"
#include "../emitter/ir/binary_expr_ir.h"

#include "../utils/log_error.h"
#include "../emitter/ir/return_ir.h"
#include "../emitter/ir/call_expr_ir.h"
#include "../emitter/ir/call_stmt_ir.h"
#include "../emitter/ir/identifier_expr_ir.h"
#include "../emitter/ir/cast_expr_ir.h"
#include "../emitter/ir/upcast_expr_ir.h"
#include "../emitter/ir/downcast_expr_ir.h"

namespace translator {

    class translator {
    public:
        explicit translator(std::unique_ptr<emitter::ir::package_ir> package_ir) :
            package_ir_(std::move(package_ir)),
            context_(std::make_unique<llvm::LLVMContext>()),
            module_(std::make_unique<llvm::Module>(package_ir_->name, *context_)),
            builder_(std::make_unique<llvm::IRBuilder<>>(*context_)) {}

        void translate();

    private:
        std::unique_ptr<emitter::ir::package_ir> package_ir_;

        std::unique_ptr<llvm::LLVMContext> context_;
        std::unique_ptr<llvm::Module> module_;
        std::unique_ptr<llvm::IRBuilder<>> builder_;

        std::map<std::string, llvm::Type*> types_;

        llvm::BasicBlock* current_allocation_block_;
        llvm::BasicBlock* current_block_;

        emitter::ir::scope_stmt_ir* current_scope_;
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
        void translate_var_stmt(emitter::ir::variable_ir* variable_ir);
        void translate_call_stmt(emitter::ir::call_stmt_ir* call_stmt);
        void translate_return_stmt(emitter::ir::return_ir* return_ir);

        llvm::Value* translate_expr(emitter::ir::expr_ir* expr);
        llvm::Constant* translate_int_expr(emitter::ir::integer_expr_ir* integer_expr);
        llvm::Constant* translate_boolean_expr(emitter::ir::boolean_expr_ir* boolean_expr);
        llvm::Value* translate_binary_expr(emitter::ir::binary_expr_ir* binary_expr);
        llvm::Value* translate_call_expr(emitter::ir::call_expr_ir* call_expr);
        llvm::Value* translate_identifier_expr(emitter::ir::identifier_expr_ir* identifier_expr);
        llvm::Value* translate_cast_expr(emitter::ir::cast_expr_ir* cast_expr);
        llvm::Value* translate_upcast_expr(emitter::ir::upcast_expr_ir* upcast_expr);
        llvm::Value* translate_downcast_expr(emitter::ir::downcast_expr_ir* downcast_expr);
    };

}

#endif //SNAKELANG_TRANSLATOR_H
