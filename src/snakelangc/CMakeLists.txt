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
        emitter/ir/package_ir.h
        emitter/ir/variable_ir.h
        emitter/ir/type.h
        emitter/ir/expr_ir.h
        emitter/ir/integer_expr_ir.h
        emitter/ir/boolean_expr_ir.h
        emitter/ir/binary_expr_ir.h
        emitter/emitter.cpp
        emitter/emitter.h
        emitter/ir/binary_operation_type.h
        translator/translator.cpp
        translator/translator.h)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(snakelangc ${SOURCE_FILES})

llvm_map_components_to_libnames(llvm_libs -16)

target_link_libraries(snakelangc ${llvm_libs})