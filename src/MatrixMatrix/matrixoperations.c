#include <stdio.h>
#include <stdlib.h>
#include <mmio.h>
#include "matrixoperations.h"

int coord_to_index(int row_coord, int col_coord, int columns){
    return (row_coord * columns) + col_coord;
}

int matrix_matrix_multiply(int matrix_dimensions_a[], int matrix_dimensions_b[], double matrix_a[], double matrix_b[], double *output_matrix){
    if(matrix_dimensions_a[1] != matrix_dimensions_b[0]){
        printf("Matrix and vector are incompatible shapes to multiply\n");
        printf("Matrix A has %d columns\n", matrix_dimensions_a[1]);
        printf("Matrix B has %d rows\n", matrix_dimensions_b[0]);
        return 1;
    }
    for(int i = 0; i < matrix_dimensions_a[0]; i++){
        for(int j = 0; j < matrix_dimensions_b[1]; j++){
            int result_index = coord_to_index(i, j, matrix_dimensions_b[1]);
            output_matrix[result_index] = 0;
            for(int k = 0; k < matrix_dimensions_b[0]; k++){
                output_matrix[result_index] += matrix_a[coord_to_index(i, k, matrix_dimensions_a[1])] * matrix_b[coord_to_index(k, j, matrix_dimensions_b[1])];
            }
        }
    }
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
