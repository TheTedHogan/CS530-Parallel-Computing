cmake_minimum_required(VERSION 3.15)
project(CS530_Parallel_Computing C)

find_package(MPI REQUIRED)
add_definitions(-DOMPI_SKIP_MPICXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp -lm")
include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/libs" "${CMAKE_SOURCE_DIR}/etc" "{CMAKE_SOURCE_DIR}/src")
include_directories(SYSTEM ${MPI_INCLUDE_PATH})


add_executable(matrixmatrix src/MatrixMatrix/matrixmatrix.c include/mmio.h libs/mmio.c src/MatrixMatrix/matrixoperations.c src/MatrixMatrix/matrixoperations.h)
add_executable(matrixmatrix_test src/MatrixMatrix/matrixmatrix_test.c include/mmio.h libs/mmio.c src/MatrixMatrix/matrixoperations.h src/MatrixMatrix/matrixoperations.c)
target_link_libraries(matrixmatrix ${MPI_C_LIBRARIES})
target_link_libraries(matrixmatrix_test ${MPI_C_LIBRARIES})

