#include <stdio.h>
#include <stdlib.h>
#include <mmio.h>
#include "matrixoperations.h"
#include <math.h>
#include <mpi.h>



int main(int argc, char *argv[]) {
    int num_procs;
    int proc_id;
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

    if(matrix_dimensions_a[0] != matrix_dimensions_a[1]){
        printf("Matrix A is not a square matrix");
    }

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

    if(matrix_dimensions_b[0] != matrix_dimensions_b[1]){
        printf("Matrix B is not a square matrix\n");
    }
    //allocate memory for matrix_b
    matrix_b = (double *) calloc(matrix_dimensions_b[0] * matrix_dimensions_b[1], sizeof(double));

    //Populate the matrix_b
    create_matrix_array(input_matrix_file_b, matrix_dimensions_b, matrix_b);

    fclose(input_matrix_file_b);

    output_matrix = calloc(matrix_dimensions_a[0] * matrix_dimensions_b[1], sizeof(double));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
    int *proc_coord;
    proc_coord = malloc(2 * sizeof(int));
    index_to_coordinate(proc_id, sqrt(num_procs), proc_coord);


    if(proc_id == 0){

        if(sqrt(num_procs) - floor(sqrt(num_procs)) != 0){
            printf("Number of processors must be a perfect square\n");
            MPI_Finalize();
            exit(1);
        }
        if(matrix_dimensions_a[0] % (int)sqrt(num_procs)){
            printf("Matrix dimensions are not divisible by %d\n", (int)sqrt(num_procs));
        }

    }
    // 25 processors and a 35X35 dimensions matrix this value would be 7, each block would be a 7X7 matrix
    int block_dim = matrix_dimensions_a[0] / (int)sqrt(num_procs);

    // Allocate the submatrix
    double *submatrix_a;
    submatrix_a = calloc(block_dim * block_dim, sizeof(double));

    // Allocate the submatrix
    double *submatrix_b;
    submatrix_b = calloc(block_dim * block_dim, sizeof(double));

    // The coordinator will initially break the A & B matricies into blocks, that will later be scattered
    if(proc_id == 0){
        int coord_start[2];
        int coord_end[2];
        // Allocate the submatrix to send
        double *submatrix_send_a;
        submatrix_send_a = calloc(block_dim * block_dim, sizeof(double));

        double *submatrix_send_b;
        submatrix_send_b = calloc(block_dim * block_dim, sizeof(double));
        MPI_Request *request;

        for(int x = 0; x < num_procs; x++){

            coord_start[0] = proc_coord[0]* block_dim;
            coord_start[1] = proc_coord[1]* block_dim;
            coord_end[0] = coord_start[0] + block_dim;
            coord_end[1] = coord_start[1] + block_dim;
            for(int i = 0; i < block_dim; i++) {
                for (int j = 0; j < block_dim; j++) {
                    submatrix_send_a[coord_to_index(i,j, block_dim)] = matrix_a[coord_to_index(coord_start[0]+i, coord_start[1]+j, matrix_dimensions_a[0])];
                    submatrix_send_b[coord_to_index(i,j, block_dim)] = matrix_b[coord_to_index(coord_start[0]+i, coord_start[1]+j, matrix_dimensions_a[0])];
                }
            }
            // Tag is 0 for matrix A and tag is 1 for matrix B
            MPI_Isend(submatrix_a, block_dim * block_dim, MPI_DOUBLE, x, 0, MPI_COMM_WORLD, request);
            MPI_Isend(submatrix_b, block_dim * block_dim, MPI_DOUBLE, x, 1, MPI_COMM_WORLD, request);
        }
    }
    int p = sqrt(num_procs);
    MPI_Status *mpi_status;
    MPI_Recv(submatrix_a, block_dim * block_dim, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, mpi_status);
    MPI_Recv(submatrix_b, block_dim * block_dim, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, mpi_status);


    //Now that the blocks have been distributed, we need skew the proc's blocks
    int skew_a_dest = coord_to_index(proc_coord[0], (proc_coord[1] - proc_coord[0]) % p, p);
    int skew_a_source = coord_to_index(proc_coord[0], (proc_coord[1] + 1) % p, p);

    int skew_b_dest = coord_to_index((proc_coord[0] - proc_coord[1]) %p, proc_coord[1], p);
    int skew_b_source = coord_to_index((proc_coord[0] + 1) % p, proc_coord[1], p);

    MPI_Sendrecv_replace(submatrix_a, block_dim * block_dim, MPI_DOUBLE, skew_a_dest, 0, skew_a_source, 0, MPI_COMM_WORLD, mpi_status);

    // With the skew compete, we can now loop through the blocks sqrt(processes) number of times, multiply the local blocks, and pass the
    // blocks to the next processor
    int *submatrix_c;
    submatrix_c = calloc(block_dim * block_dim, sizeof(double));


    /*
    if(proc_id == 0) {
        MPI_Send(submatrix_a, block_dim * block_dim, MPI_DOUBLE, skew_a_dest, 0, MPI_COMM_WORLD);
        MPI_Recv(submatrix_a, block_dim * block_dim, MPI_DOUBLE, skew_a_soruce, 0, MPI_COMM_WORLD, mpi_status);
    } else
    {
        MPI_Recv(submatrix_a, block_dim * block_dim, MPI_DOUBLE, skew_a_soruce, 0, MPI_COMM_WORLD, mpi_status);
        MPI_Send(submatrix_a, block_dim * block_dim, MPI_DOUBLE, skew_a_dest, 0, MPI_COMM_WORLD);
    }

    if(proc_id == 0) {
        MPI_Send(submatrix_b, block_dim * block_dim, MPI_DOUBLE, skew_b_dest, 0, MPI_COMM_WORLD);
        MPI_Recv(submatrix_b, block_dim * block_dim, MPI_DOUBLE, skew_b_soruce, 0, MPI_COMM_WORLD, mpi_status);
    } else
    {
        MPI_Recv(submatrix_b, block_dim * block_dim, MPI_DOUBLE, skew_b_soruce, 0, MPI_COMM_WORLD, mpi_status);
        MPI_Send(submatrix_b, block_dim * block_dim, MPI_DOUBLE, skew_b_dest, 0, MPI_COMM_WORLD);
    }
     */

    matrix_matrix_multiply(matrix_dimensions_a, matrix_dimensions_b, matrix_a, matrix_b, output_matrix);


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
