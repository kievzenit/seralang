cmake_minimum_required(VERSION 3.27)
project(seralang)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/serac)
add_subdirectory(tests)
