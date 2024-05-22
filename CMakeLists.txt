cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 11)

project (mmu)

SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.c")
add_executable(mmu ${SRC})

#target_compile_definitions(mmu PUBLIC DEBUG)
#target_compile_definitions(mmu PUBLIC VERBOSE)