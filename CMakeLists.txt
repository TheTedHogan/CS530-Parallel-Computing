cmake_minimum_required(VERSION 3.22)
project(CS530_Parallel_Computing C)

set(CMAKE_C_STANDARD 99)
include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/libs" "${CMAKE_SOURCE_DIR}/etc")

add_executable(matrixvector src/MatrixVector/matrixvector.c include/mmio.h libs/mmio.c)

add_executable(matrixmatrix src/MatrixMatrix/matrixmatrix.c include/mmio.h libs/mmio.c)



add_executable(fibonacci src/fibonacci/fibonacci.c)

add_executable(fibonacci_test src/fibonacci/fibonacci_test.c)
