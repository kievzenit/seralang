cmake_minimum_required(VERSION 3.27)
project(snakelangc)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES main.cpp
        utils/log_error.h
        utils/log_error.cpp
        lexer/token_type.h
        lexer/token.h
        lexer/lexer.cpp
        lexer/lexer.h
        parser/parser.cpp
        parser/parser.h
        parser/ast/ast_node.h
        parser/ast/translation_ast.h
        parser/ast/common/binary_operation.h
        parser/ast/common/func_param.h
        parser/ast/stmts/package_stmt.h
        parser/ast/stmts/top_stmt.h
        parser/ast/stmts/let_stmt.h
        parser/ast/stmts/func_decl_stmt.h
        parser/ast/stmts/stmt.h
        parser/ast/stmts/scope_stmt.h
        parser/ast/stmts/return_stmt.h
        parser/ast/stmts/call_stmt.h
        parser/ast/stmts/assignment_stmt.h
        parser/ast/exprs/expr.h
        parser/ast/exprs/integer_expr.h
        parser/ast/exprs/boolean_expr.h
        parser/ast/exprs/identifier_expr.h
        parser/ast/exprs/binary_expr.h
        parser/ast/exprs/identifier_expr.h
        parser/ast/exprs/call_expr.h
        emitter/emitter.cpp
        emitter/emitter.h
        emitter/emitter.exprs.cpp
        emitter/emitter.stmts.cpp
        emitter/emitter.globals.cpp
        emitter/emitter.funcs.cpp
        emitter/ir/package_ir.h
        emitter/ir/variable_ir.h
        emitter/ir/func_decl_ir.h
        emitter/ir/types/type.h
        emitter/ir/types/type.cpp
        emitter/ir/types/func_param_ir.h
        emitter/ir/types/func_type.h
        emitter/ir/stmts/assignment_stmt_ir.h
        emitter/ir/stmts/return_stmt_ir.h
        emitter/ir/stmts/stmt_ir.h
        emitter/ir/stmts/scope_stmt_ir.h
        emitter/ir/stmts/call_stmt_ir.h
        emitter/ir/exprs/expr_ir.h
        emitter/ir/exprs/integer_expr_ir.h
        emitter/ir/exprs/boolean_expr_ir.h
        emitter/ir/exprs/binary_expr_ir.h
        emitter/ir/types/binary_operation_type.h
        emitter/ir/exprs/logical_expr_ir.h
        emitter/ir/exprs/relational_expr_ir.h
        emitter/ir/exprs/arithmetic_expr_ir.h
        emitter/ir/exprs/bitwise_expr_ir.h
        emitter/ir/exprs/call_expr_ir.h
        emitter/ir/exprs/identifier_expr_ir.h
        emitter/ir/exprs/argument_expr_ir.h
        emitter/ir/exprs/cast_expr_ir.h
        emitter/ir/exprs/upcast_expr_ir.h
        emitter/ir/exprs/downcast_expr_ir.h
        translator/translator.cpp
        translator/translator.h
        compiler/compiler.cpp
        compiler/compiler.h
        compiler/compile_options.h
        compiler/emit_type.h)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_compile_definitions(DEBUG)
add_executable(snakelangc ${SOURCE_FILES})

llvm_map_components_to_libnames(llvm_libs -16)

target_link_libraries(snakelangc ${llvm_libs})