#include <stdio.h>
#include <stdlib.h>
#include <mmio.h>

int matrix_vector_multiply(int matrix_dimensions[], int vector_dimensions[], double matrix[], double vector[], double *output_matrix){
    if(matrix_dimensions[1] != vector_dimensions[0]){
        printf("Matrix and vector are incompatible shapes to multiply");
        printf("Matrix has %d columns", matrix_dimensions[1]);
        printf("Vector has %d rows", vector_dimensions[0]);
        return 1;
    }
    for(int i = 0; i < matrix_dimensions[1]; i++){
        output_matrix[i] = 0;
        for(int j = 0; j < vector_dimensions[0]; j++){
            output_matrix[i] += (matrix[(i*matrix_dimensions[1]) +j] * vector[j]);
        }
    }
}

void get_mmio_dimensions(FILE *f, int* dimensions_out){
    MM_typecode matcode;
    int rows, columns, num_non_zero;

    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not read banner for input file.\n");
        exit(1);
    }

    mm_read_mtx_crd_size(f, &rows, &columns, &num_non_zero);
    //printf("rows %d columns %d non zero %d\n", rows, columns, num_non_zero);

    //subtract 1 from the dimensions to adjusted from 1 index to 0 based indexing
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


int main(int argc, char *argv[]) {

    MM_typecode matcode;
    FILE *input_matrix_file;
    FILE *input_vector_file;
    FILE *output_file;
    int matrix_dimensions[3];
    int vector_dimensions[3];
    double *matrix;
    double *vector;
    double *output_matrix;

    if ((input_matrix_file = fopen(argv[1], "r")) == NULL) {
        printf("Failed to open input matrix file");
        exit(1);
    }
    // Fetch the dimensions for the matrix
    get_mmio_dimensions(input_matrix_file, matrix_dimensions);

    //allocate memory for matrix
    //int matrix[matrix_dimensions[0]*matrix_dimensions[1]] = {0};
    matrix = (double *) calloc(matrix_dimensions[0] * matrix_dimensions[1], sizeof(double));

    //Populate the matrix
    create_matrix_array(input_matrix_file, matrix_dimensions, matrix);

    fclose(input_matrix_file);


    if ((input_vector_file = fopen(argv[2], "r")) == NULL) {
        printf("Failed to open input vector file");
        exit(1);
    }
    // Fetch the dimensions for the matrix
    get_mmio_dimensions(input_vector_file, vector_dimensions);

    //allocate memory for vector
    vector = (double *) calloc(matrix_dimensions[0], sizeof(double));

    //Populate the matrix
    create_matrix_array(input_vector_file, vector_dimensions, vector);

    fclose(input_vector_file);

    output_matrix = calloc(matrix_dimensions[0], sizeof(double));

    matrix_vector_multiply(matrix_dimensions, vector_dimensions, matrix, vector, output_matrix);

    for(int i=0; i < matrix_dimensions[0]; i ++){
        printf("%g\n", output_matrix[i]);
    }

    //Release memory
    free(matrix);
    free(vector);
    return(0);
}
