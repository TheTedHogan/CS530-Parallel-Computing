#include <stdio.h>
#include <stdlib.h>
#include "matrixoperations.h"



int main(int argc, char *argv[]) {

    //MM_typecode matcode;
    FILE *input_matrix_file;
    FILE *input_vector;
    FILE *output_file;
    int matrix_dimensions[3];
    int vector_dimensions[3];
    double *matrix;
    double *vector;
    double *output_vector;

    if(argc != 4){
        printf("Usage is: matrixvector matrix_file vector_file output_file");
    }
    if ((input_matrix_file = fopen(argv[1], "r")) == NULL) {
        printf("Failed to open input matrix file\n");
        exit(1);
    }
    // Fetch the dimensions for the matrix
    get_mmio_dimensions(input_matrix_file, matrix_dimensions);

    //allocate memory for the matrix
    matrix = (double *) calloc(matrix_dimensions[0] * matrix_dimensions[1], sizeof(double));

    //Populate the first matrix
    create_matrix_array(input_matrix_file, matrix_dimensions, matrix);

    fclose(input_matrix_file);


    if ((input_vector = fopen(argv[2], "r")) == NULL) {
        printf("argv2: %s\n", argv[2]);
        printf("Failed to open input vector file\n");
        exit(1);
    }
    // Fetch the dimensions for the vector
    get_mmio_dimensions(input_vector, vector_dimensions);

    //allocate memory for vector
    vector = (double *) calloc(vector_dimensions[0] * vector_dimensions[1], sizeof(double));

    //Populate the vector
    create_matrix_array(input_vector, vector_dimensions, vector);

    fclose(input_vector);

    output_vector = calloc(matrix_dimensions[0] * vector_dimensions[1], sizeof(double));

    matrix_vector_multiply(matrix_dimensions, vector_dimensions, matrix, vector, output_vector);

    /* uncomment to print out resut matrix to stdout
     * most results will be too big to be feasibly understood on std out, so this was not made a cli option

    for(int i=0; i < matrix_dimensions[0]; i ++){
        for (int j = 0; j < vector_dimensions[1]; j++){
            printf("%g\t", output_vector[coord_to_index(i, j, vector_dimensions[1])]);
        }
        printf("\n");
    }
    */

    int output_dimensions[3] = {matrix_dimensions[0], 1, matrix_dimensions[0] };

    //Open file to write out
    if ((output_file = fopen(argv[3], "w")) == NULL) {
        printf("Failed to open output vector file\n");
        exit(1);
    }

    write_matrix_to_file(output_file, output_dimensions, output_vector);
    fclose(output_file);
    //Release memory
    free(matrix);
    free(vector);
    return(0);
}
