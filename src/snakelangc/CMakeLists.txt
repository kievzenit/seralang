cmake_minimum_required(VERSION 3.27)
project(snakelangc)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES main.cpp
        lexer/token_type.h
        lexer/token.h
        lexer/lexer.cpp
        lexer/lexer.h
        utils/log_error.h
        utils/log_error.cpp
        parser/parser.cpp
        parser/parser.h
        parser/ast/ast_node.h
        parser/ast/translation_ast.h
        parser/ast/package_stmt.h
        parser/ast/top_stmt.h
        parser/ast/let_stmt.h
        parser/ast/expr.h
        parser/ast/integer_expr.h
        parser/ast/boolean_expr.h
        parser/ast/identifier_expr.h
        parser/ast/binary_operation.h
        parser/ast/binary_expr.h
        parser/ast/func_decl_stmt.h
        parser/ast/stmt.h
        parser/ast/scope_stmt.h
        parser/ast/return_stmt.h
        parser/ast/identifier_expr.h
        parser/ast/call_expr.h
        parser/ast/call_stmt.h
        parser/ast/func_param.h
        parser/ast/assignment_stmt.h
        emitter/ir/package_ir.h
        emitter/variable_ir.h
        emitter/ir/stmts/assignment_stmt_ir.h
        emitter/ir/types/type.h
        emitter/ir/types/func_param_ir.h
        emitter/ir/types/func_type.h
        emitter/ir/exprs/expr_ir.h
        emitter/ir/exprs/integer_expr_ir.h
        emitter/ir/exprs/boolean_expr_ir.h
        emitter/ir/exprs/binary_expr_ir.h
        emitter/ir/types/binary_operation_type.h
        emitter/ir/func_decl_ir.h
        emitter/ir/stmts/return_stmt_ir.h
        emitter/ir/stmts/stmt_ir.h
        emitter/ir/stmts/scope_stmt_ir.h
        emitter/emitter.cpp
        emitter/emitter.h
        emitter/ir/exprs/call_expr_ir.h
        emitter/ir/exprs/identifier_expr_ir.h
        emitter/ir/exprs/argument_expr_ir.h
        emitter/ir/stmts/call_stmt_ir.h
        emitter/ir/types/type.cpp
        emitter/ir/exprs/cast_expr_ir.h
        emitter/ir/exprs/upcast_expr_ir.h
        emitter/ir/exprs/downcast_expr_ir.h
        translator/translator.cpp
        translator/translator.h
        compiler/compiler.cpp
        compiler/compiler.h
        compiler/compile_options.h
        compiler/emit_type.h
        emitter/emitter.exprs.cpp
        emitter/emitter.stmts.cpp
        emitter/emitter.globals.cpp
        emitter/emitter.funcs.cpp)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_compile_definitions(DEBUG)
add_executable(snakelangc ${SOURCE_FILES})

llvm_map_components_to_libnames(llvm_libs -16)

target_link_libraries(snakelangc ${llvm_libs})