#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <omp.h>
#include <cuda.h>


int coord_to_index(int row_coord, int col_coord, int columns){
    return (row_coord * columns) + col_coord;
}


void random_square_matrix(int n, float *matrix_out){

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            matrix_out[coord_to_index(i, j, n)] = rand();
        }
    }
}

__global__ void matrix_matrix_multiply(int matrix_dimensions_a[], int matrix_dimensions_b[], float matrix_a[], float matrix_b[], float *output_matrix){
    int i, j, k;


    if(matrix_dimensions_a[1] != matrix_dimensions_b[0]){
        printf("Matrices are incompatible shapes to multiply\n");
        printf("Matrix A has %d columns\n", matrix_dimensions_a[1]);
        printf("Matrix B has %d rows\n", matrix_dimensions_b[0]);
    }

    for(i = 0; i < matrix_dimensions_a[0]; i++){
        for(j = 0; j < matrix_dimensions_b[1]; j++){
            for(k = 0; k < matrix_dimensions_b[0]; k++){
                output_matrix[(i*matrix_dimensions_b[1]) + j] = 0;
            }
        }
    }


    for(i = 0; i < matrix_dimensions_a[0]; i++){
        for(j = 0; j < matrix_dimensions_b[1]; j++){
            for(k = 0; k < matrix_dimensions_b[0]; k++){
                output_matrix[(i*matrix_dimensions_b[1]) + j] += matrix_a[(i*matrix_dimensions_a[1]) + j] * matrix_b[(i*matrix_dimensions_b[1]) + j];
            }
        }
    }

}
//
//void get_mmio_dimensions(FILE *f, int* dimensions_out){
//    //Dimensions will be placed in the dimensions_out parameter with the following form:
//    // dimensions_out[0] = rows
//    // dimensions_out[1] = columns
//    // dimensions_out[2] = number of non-zero coordinates
//    MM_typecode matcode;
//    int rows, columns, num_non_zero;
//
//    if (mm_read_banner(f, &matcode) != 0)
//    {
//        printf("Could not read banner for input file.\n");
//        exit(1);
//    }
//
//    if(!mm_is_coordinate(matcode)){
//        printf("This application only supports MMIO files in the coordinate format");
//        exit(1);
//    }
//
//    mm_read_mtx_crd_size(f, &rows, &columns, &num_non_zero);
//
//    dimensions_out[0] = rows ;
//    dimensions_out[1] = columns ;
//    dimensions_out[2] = num_non_zero;
//
//}

void create_matrix_array(FILE *f, int * dimensions, float *matrix_out){
    //subtract 1 from the demi

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

void write_matrix_to_file(FILE *f, int dimensions[], float matrix_out[]){
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


int main(int argc, char *argv[]) {

    //MM_typecode matcode;
    FILE *input_matrix_file_a;
    FILE *input_matrix_file_b;
    FILE *output_file;
    int matrix_dimensions_a[3];
    int matrix_dimensions_b[3];
    float *matrix_a;
    float *matrix_b;
    float *output_matrix;
    int n = 10;
//    if(argc != 4){
//        printf("Usage is: matrixmatrix matrix_a_file matrix_b_file output_file");
//    }
//    if ((input_matrix_file_a = fopen(argv[1], "r")) == NULL) {
//        printf("Failed to open input matrix_a file\n");
//        exit(1);
//    }
    // Fetch the dimensions for the first matrix_a
    //get_mmio_dimensions(input_matrix_file_a, matrix_dimensions_a);

    //allocate memory for first matrix_a
    //int matrix_a[matrix_dimensions_a[0]*matrix_dimensions_a[1]] = {0};
    cudaMallocManaged(&matrix_a,n*n * sizeof(float));

    //Populate the first matrix_a
    //create_matrix_array(input_matrix_file_a, matrix_dimensions_a, matrix_a);
    random_square_matrix(n, matrix_a);

    //fclose(input_matrix_file_a);


//    if ((input_matrix_file_b = fopen(argv[2], "r")) == NULL) {
//        printf("Failed to open input matrix_b file\n");
//        exit(1);
//    }
    // Fetch the dimensions for the matrix_b
   // get_mmio_dimensions(input_matrix_file_b, matrix_dimensions_b);

    //allocate memory for matrix_b
    cudaMallocManaged(&matrix_b,n *n * sizeof(float));

    //Populate the matrix_b
    //create_matrix_array(input_matrix_file_b, matrix_dimensions_b, matrix_b);
    random_square_matrix(n, matrix_b);

    //fclose(input_matrix_file_b);

    cudaMallocManaged(&output_matrix, n*n * sizeof(float));
    dim3 gridDim = 1;
    dim3 blockDim = 1000;
    matrix_matrix_multiply<<<gridDim,blockDim>>>(matrix_dimensions_a, matrix_dimensions_b, matrix_a, matrix_b, output_matrix);

    // for(int i=0; i < matrix_dimensions_a[0]; i ++){
    //     for (int j = 0; j < matrix_dimensions_b[1]; j++){
    //         printf("%g\t", output_matrix[coord_to_index(i, j, matrix_dimensions_b[1])]);
    //     }
    //     printf("\n");
    // }
    cudaDeviceSynchronize();
    int output_dimensions[3] = {matrix_dimensions_a[0], matrix_dimensions_b[1], matrix_dimensions_a[0] * matrix_dimensions_b[1] };

    //Open file to write out
    if ((output_file = fopen(argv[3], "w")) == NULL) {
        printf("Failed to open output matrix file\n");
        exit(1);
    }

    write_matrix_to_file(output_file, output_dimensions, output_matrix);
    //fclose(output_file);
    //Release memory
    //free(matrix_a);
    //free(matrix_b);
    return(0);
}
