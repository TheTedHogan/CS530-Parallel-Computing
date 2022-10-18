#include <stdio.h>
#include <stdlib.h>
#include <mmio.h>
#include "matrixoperations.h"
#include <time.h>

int coord_to_index(int row_coord, int col_coord, int columns){
    return (row_coord * columns) + col_coord;
}

int matrix_matrix_multiply(int matrix_dimensions_a[], int matrix_dimensions_b[], double matrix_a[], double matrix_b[], double *output_matrix){
    int i, j, k, numThreads;
    struct timeval startTime, endTime;


    if(matrix_dimensions_a[1] != matrix_dimensions_b[0]){
        printf("Matrices are incompatible shapes to multiply\n");
        printf("Matrix A has %d columns\n", matrix_dimensions_a[1]);
        printf("Matrix B has %d rows\n", matrix_dimensions_b[0]);
        return 1;
    }

    clock_t begin = clock();

    #pragma omp parallel default(none) shared(matrix_dimensions_a, matrix_dimensions_b, matrix_a, matrix_b, output_matrix, numThreads) private(i, j, k)
    {
        #pragma omp single
        {
          numThreads = omp_get_num_threads();
        }
        #pragma omp for schedule(static) collapse(3)
        for(i = 0; i < matrix_dimensions_a[0]; i++){
            for(j = 0; j < matrix_dimensions_b[1]; j++){
                for(k = 0; k < matrix_dimensions_b[0]; k++){
                    output_matrix[coord_to_index(i, j, matrix_dimensions_b[1])] = 0;
                }
            }
        }
    }

    #pragma omp parallel default(none) shared(matrix_dimensions_a, matrix_dimensions_b, matrix_a, matrix_b, output_matrix, numThreads) private(i, j, k)
    {
      #pragma omp single
      {
        numThreads = omp_get_num_threads();
      }
      #pragma omp for schedule(static) collapse(3)
      for(i = 0; i < matrix_dimensions_a[0]; i++){
          for(j = 0; j < matrix_dimensions_b[1]; j++){
             for(k = 0; k < matrix_dimensions_b[0]; k++){
                  output_matrix[coord_to_index(i, j, matrix_dimensions_b[1])] += matrix_a[coord_to_index(i, k, matrix_dimensions_a[1])] * matrix_b[coord_to_index(k, j, matrix_dimensions_b[1])];
              }
          }
      }
    }

    clock_t end = clock();

    double timeElapsed = (double)(end - begin) / CLOCKS_PER_SEC / 1000;
    // printf("Time elapsed for matrix multiplications is %0.2f seconds\n", timeElapsed);
    printf("%d\t%0.6f\t\n", numThreads, timeElapsed);
    return(0);
}

void get_mmio_dimensions(FILE *f, int* dimensions_out){
    //Dimensions will be placed in the dimensions_out parameter with the following form:
    // dimensions_out[0] = rows
    // dimensions_out[1] = columns
    // dimensions_out[2] = number of non-zero coordinates
    MM_typecode matcode;
    int rows, columns, num_non_zero;

    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not read banner for input file.\n");
        exit(1);
    }

    if(!mm_is_coordinate(matcode)){
        printf("This application only supports MMIO files in the coordinate format");
        exit(1);
    }

    mm_read_mtx_crd_size(f, &rows, &columns, &num_non_zero);

    dimensions_out[0] = rows ;
    dimensions_out[1] = columns ;
    dimensions_out[2] = num_non_zero;

}

void create_matrix_array(FILE *f, int * dimensions, double *matrix_out){
    //subtract 1 from the demi
    int rows = dimensions[0];
    int columns = dimensions[1];
    int num_non_zero = dimensions[2];
    int *row_index;
    int *column_index;
    double *val;

    row_index = (int *) malloc(num_non_zero * sizeof(int));
    column_index = (int *) malloc(num_non_zero * sizeof(int));
    val = (double *) malloc(num_non_zero * sizeof(double));

    for(int i = 0; i < num_non_zero; i++){
        fscanf(f, "%d %d %lg\n", &row_index[i], &column_index[i], &val[i]);
        row_index[i]--;
        column_index[i]--;
        matrix_out[(row_index[i] * columns) + column_index[i]] = val[i];
    }

}

int write_matrix_to_file(FILE *f, int dimensions[], double matrix_out[]){
    MM_typecode matout_code;

    mm_initialize_typecode(&matout_code);
    mm_set_matrix(&matout_code);
    mm_set_coordinate(&matout_code);
    mm_set_real(&matout_code);

    mm_write_banner(f, matout_code);
    mm_write_mtx_crd_size(f, dimensions[0], dimensions[1], dimensions[2]);

    for(int i=0; i < dimensions[0]; i ++){
        for (int j = 0; j < dimensions[1]; j++){
            fprintf(f, "%d %d %g\n", i+1, j+1, matrix_out[coord_to_index(i, j, dimensions[1])]);
        }
    }
}
