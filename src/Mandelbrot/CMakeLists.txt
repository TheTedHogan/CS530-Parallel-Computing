cmake_minimum_required(VERSION 3.15)
project(CS530_Parallel_Computing C CUDA)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp -lm")
include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/libs" "${CMAKE_SOURCE_DIR}/etc" "{CMAKE_SOURCE_DIR}/src")
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_executable(mandelbrot_serial mandelbrot_serial.c)
add_executable(mandelbrot_cuda mandelbrot_cuda.cu)
add_executable(mandelbrot_openmp mandelbrot_openmp.c)