#include <stdio.h>
#include <stdlib.h>
#include <mmio.h>
#include "matrixoperations.h"
#include <omp.h>
#include <sys/time.h>

int coord_to_index(int row_coord, int col_coord, int columns){
    return (row_coord * columns) + col_coord;
}
int matrix_vector_multiply(int matrix_dimensions[], int vector_dimensions[], double matrix[], double vector[], double *output_matrix){
    int i, j, numThreads;
    struct timeval startTime, endTime;


    if(matrix_dimensions[1] != vector_dimensions[0]){
        printf("Matrix and vector are incompatible shapes to multiply\n");
        printf("Matrix has %d columns\n", matrix_dimensions[1]);
        printf("Vector has %d rows\n", vector_dimensions[0]);
        return 1;
    }
    if(vector_dimensions[1] != 1){
        printf("Vector argument is not a properly formatted vector\v");
        return 1;
    }

    gettimeofday(&startTime, 0);

    #pragma omp parallel default(none) shared(matrix_dimensions, vector_dimensions, matrix, vector, output_matrix, numThreads) private(i, j)
    {
      #pragma omp single
      {
        numThreads = omp_get_num_threads();
      }
      #pragma omp for schedule(static)
        for(i = 0; i < matrix_dimensions[0]; i++){
            output_matrix[i] = 0;

            for(j = 0; j < vector_dimensions[0]; j++){
                output_matrix[i] = 0;
            }
        }
    }

    #pragma omp parallel default(none) shared(matrix_dimensions, vector_dimensions, matrix, vector, output_matrix, numThreads) private(i, j)
    {
      #pragma omp single
      {
        numThreads = omp_get_num_threads();
      }
      #pragma omp for schedule(static) collapse(2)
      for(i = 0; i < matrix_dimensions[0]; i++){
          for(j = 0; j < vector_dimensions[0]; j++){
              output_matrix[i] += (matrix[coord_to_index(i, j, matrix_dimensions[1])] * vector[j]);
          }
      }
    }

    gettimeofday(&endTime, 0);
    double timeElapsed = (endTime.tv_sec - startTime.tv_sec) * 1.0f + (endTime.tv_usec - startTime.tv_usec) / 1000000.0f;
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
