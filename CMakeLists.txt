cmake_minimum_required(VERSION 3.23)
project(CS530_Parallel_Computing C)

set(CMAKE_C_STANDARD 99)

add_executable(CS530_Parallel_Computing src/main.c)

add_executable(matrixvector src/MatrixVector/matrixvector.c)

add_executable(matrixmatrix src/MatrixMatrix/matrixmatrix.c)
