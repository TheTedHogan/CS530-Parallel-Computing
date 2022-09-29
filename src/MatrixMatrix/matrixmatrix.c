#include <stdio.h>
#include <stdlib.h>
#include <mmio.h>
#include "matrixoperations.h"
#include <omp.h>

int main(int argc, char *argv[]) {

    MM_typecode matcode;
    FILE *input_matrix_file_a;
    FILE *input_matrix_file_b;
    FILE *output_file;
    int matrix_dimensions_a[3];
    int matrix_dimensions_b[3];
    double *matrix_a;
    double *matrix_b;
    double *output_matrix;

    if(argc != 4){
        printf("Usage is: matrixmatrix matrix_a_file matrix_b_file output_file");
    }
    if ((input_matrix_file_a = fopen(argv[1], "r")) == NULL) {
        printf("Failed to open input matrix_a file\n");
        exit(1);
    }
    // Fetch the dimensions for the first matrix_a
    get_mmio_dimensions(input_matrix_file_a, matrix_dimensions_a);

    //allocate memory for first matrix_a
    //int matrix_a[matrix_dimensions_a[0]*matrix_dimensions_a[1]] = {0};
    matrix_a = (double *) calloc(matrix_dimensions_a[0] * matrix_dimensions_a[1], sizeof(double));

    //Populate the first matrix_a
    create_matrix_array(input_matrix_file_a, matrix_dimensions_a, matrix_a);

    fclose(input_matrix_file_a);


    if ((input_matrix_file_b = fopen(argv[2], "r")) == NULL) {
        printf("Failed to open input matrix_b file\n");
        exit(1);
    }
    // Fetch the dimensions for the matrix_b
    get_mmio_dimensions(input_matrix_file_b, matrix_dimensions_b);

    //allocate memory for matrix_b
    matrix_b = (double *) calloc(matrix_dimensions_b[0] * matrix_dimensions_b[1], sizeof(double));

    //Populate the matrix_a
    create_matrix_array(input_matrix_file_b, matrix_dimensions_b, matrix_b);

    fclose(input_matrix_file_b);

    output_matrix = calloc(matrix_dimensions_a[0] * matrix_dimensions_b[1], sizeof(double));

    matrix_matrix_multiply(matrix_dimensions_a, matrix_dimensions_b, matrix_a, matrix_b, output_matrix);

    // for(int i=0; i < matrix_dimensions_a[0]; i ++){
    //     for (int j = 0; j < matrix_dimensions_b[1]; j++){
    //         printf("%g\t", output_matrix[coord_to_index(i, j, matrix_dimensions_b[1])]);
    //     }
    //     printf("\n");
    // }

    int output_dimensions[3] = {matrix_dimensions_a[0], matrix_dimensions_b[1], matrix_dimensions_a[0] * matrix_dimensions_b[1] };

    //Open file to write out
    if ((output_file = fopen(argv[3], "w")) == NULL) {
        printf("Failed to open output matrix file\n");
        exit(1);
    }

    write_matrix_to_file(output_file, output_dimensions, output_matrix);
    fclose(output_file);
    //Release memory
    free(matrix_a);
    free(matrix_b);
    return(0);
}
