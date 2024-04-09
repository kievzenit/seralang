cmake_minimum_required(VERSION 3.27)
project(snakelangc)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES main.cpp
        lexer/token_type.h
        lexer/token.h
        lexer/lexer.cpp
        lexer/lexer.h
        utils/log_error.h
        utils/log_error.cpp)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(snakelangc ${SOURCE_FILES})

llvm_map_components_to_libnames(llvm_libs -16)

target_link_libraries(snakelangc ${llvm_libs})