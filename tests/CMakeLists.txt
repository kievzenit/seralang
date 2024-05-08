cmake_minimum_required(VERSION 3.27)
project(serac_tests)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs -16)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
add_executable(serac_test ${TEST_SOURCES} ${COMPILER_SOURCES})

target_include_directories(serac PUBLIC ../src/serac)
target_link_libraries(serac_test gtest ${llvm_libs} serac_lib)

include(GoogleTest)
gtest_discover_tests(serac_test)
