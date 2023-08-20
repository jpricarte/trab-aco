cmake_minimum_required(VERSION 3.27)
project(aco)

set(CMAKE_CXX_STANDARD 17)

add_executable(aco main.cpp)
add_executable(runner runner.cpp)
add_executable(graph-gen graph-gen.cpp)
