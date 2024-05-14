cmake_minimum_required(VERSION 3.27)
project(serac)

set(CMAKE_CXX_STANDARD 23)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_compile_definitions(DEBUG)
file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES false *.h *.cpp)
add_executable(serac ${SOURCE_FILES})
add_library(serac_lib STATIC ${SOURCE_FILES})

llvm_map_components_to_libnames(llvm_libs -18)

target_link_libraries(serac ${llvm_libs})
target_link_libraries(serac_lib ${llvm_libs})
