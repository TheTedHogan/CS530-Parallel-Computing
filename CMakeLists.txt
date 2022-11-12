cmake_minimum_required(VERSION 3.15)
project(CS530_Parallel_Computing C)

find_package(MPI REQUIRED)
add_definitions(-DOMPI_SKIP_MPICXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp -lm")
include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/libs" "${CMAKE_SOURCE_DIR}/etc" "{CMAKE_SOURCE_DIR}/src")
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_executable(matrixvector src/MatrixVector/matrixvector.c include/mmio.h libs/mmio.c src/MatrixVector/matrixoperations.c src/MatrixVector/matrixoperations.h)
add_executable(matrixvector_test src/MatrixVector/matrixvector_test.c include/mmio.h libs/mmio.c src/MatrixVector/matrixoperations.h src/MatrixVector/matrixoperations.c)

add_executable(matrixmatrix src/MatrixMatrix/matrixmatrix.c include/mmio.h libs/mmio.c src/MatrixMatrix/matrixoperations.c src/MatrixMatrix/matrixoperations.h)
add_executable(matrixmatrix_test src/MatrixMatrix/matrixmatrix_test.c include/mmio.h libs/mmio.c src/MatrixMatrix/matrixoperations.h src/MatrixMatrix/matrixoperations.c)

add_executable(fibonacci src/fibonacci/fibonacci.c)
add_executable(fibonacci_omp src/fibonacci/fibonacci_omp.c)
add_executable(fibonacci_test src/fibonacci/fibonacci_test.c)


add_executable(leibniz src/LeibnizSeries/leibniz_series.c)
target_link_libraries(leibniz m)
add_executable(leibniz_test src/LeibnizSeries/leibniz_series_test.c)

add_executable(montecarlo src/MonteCarlo/monte_carlo.c)
target_link_libraries(montecarlo m)
add_executable(montecarlo_test src/MonteCarlo/monte_carlo_test.c)

add_executable(matrixmatrixcannon src/MatrixMatrixCannon/matrixmatrix.c include/mmio.h libs/mmio.c src/MatrixMatrixCannon/matrixoperations.c src/MatrixMatrixCannon/matrixoperations.h)
add_executable(matrixmatrixcannon_test src/MatrixMatrixCannon/matrixmatrix_test.c include/mmio.h libs/mmio.c src/MatrixMatrixCannon/matrixoperations.h src/MatrixMatrixCannon/matrixoperations.c)
target_link_libraries(matrixmatrixcannon ${MPI_C_LIBRARIES})
target_link_libraries(matrixmatrixcannon_test ${MPI_C_LIBRARIES})
