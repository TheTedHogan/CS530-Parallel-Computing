cmake_minimum_required(VERSION 3.22)
project(CS530_Parallel_Computing C)

set(CMAKE_C_STANDARD 99)

add_executable(CS530_Parallel_Computing src/main.c)

add_executable(fibonacci src/fibonacci/fibonacci.c)

add_executable(fibonacci_test src/fibonacci/fibonacci_test.c)
