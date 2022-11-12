#include <stdio.h>
#include <stdlib.h>
#include <mmio.h>
#include "matrixoperations.h"
#include <math.h>
#include <mpi.h>

void testfn(){
            for(int k =0; k < 10; k++){

            }

}


int main(int argc, char *argv[]) {
    int num_procs;
    int proc_id;
    int n;
    MM_typecode matcode;
    FILE *input_matrix_file_a;
    FILE *input_matrix_file_b;
    FILE *output_file;
    double *matrix_a;
    double *matrix_b;
    double *output_matrix;

    if(argc != 3){
        printf("Usage is: matrixmatrix n output_file");
    }
    n = atoi(argv[1]);



    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
    int *proc_coord;
    proc_coord = malloc(2 * sizeof(int));
    index_to_coordinate(proc_id, sqrt(num_procs), proc_coord);


    if(proc_id == 0){
        output_matrix = calloc(n * n, sizeof(double));

        if(sqrt(num_procs) - floor(sqrt(num_procs)) != 0){
            printf("Number of processors must be a perfect square\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            exit(1);
        }

        if(n % (int)sqrt(num_procs)){
            printf("Matrix dimensions: %d, are not divisible by %d\n", n, (int)sqrt(num_procs));
        }

        //allocate memory for first matrix_a
        matrix_a = (double *) calloc(n * n, sizeof(double));
        matrix_b = (double *) calloc(n * n, sizeof(double));
        random_square_matrix(n, matrix_a);
        random_square_matrix(n, matrix_b);


    }
    // 25 processors and a 35X35 dimensions matrix this value would be 7, each block would be a 7X7 matrix
    int block_dim = n / (int)sqrt(num_procs);

    // Allocate the submatrix
    double *submatrix_a;
    submatrix_a = calloc(block_dim * block_dim, sizeof(double));

    // Allocate the submatrix
    double *submatrix_b;
    submatrix_b = calloc(block_dim * block_dim, sizeof(double));

    // Allocate the submatrices to send
    // They will be reorganized into blocks that can be scattered to th
    double *submatrix_send_a;
    double *submatrix_send_b;
    submatrix_send_a = calloc(n * n, sizeof(double));

    submatrix_send_b = calloc(n * n, sizeof(double));


    // The coordinator will initially break the A & B matricies into blocks, that will later be scattered
    if(proc_id == 0){
        int coord_start[2];
        int coord_end[2];

        for(int x = 0; x < num_procs; x++){
            MPI_Request *request;
            coord_start[0] = proc_coord[0]* block_dim;
            coord_start[1] = proc_coord[1]* block_dim;
            coord_end[0] = coord_start[0] + block_dim;
            coord_end[1] = coord_start[1] + block_dim;

            for(int i = coord_start[0]; i < coord_end[0]; i++) {
                for (int j = coord_start[1]; j < coord_end[1]; j++) {
                    submatrix_send_a[coord_to_index(i,j, n)] = matrix_a[coord_to_index(coord_start[0]+i, coord_start[1]+j, n)];
                    submatrix_send_b[coord_to_index(i,j, n)] = matrix_b[coord_to_index(coord_start[0]+i, coord_start[1]+j, n)];
                }
            }

        }

    }
    MPI_Scatter(submatrix_send_a, block_dim * block_dim, MPI_DOUBLE, submatrix_a, block_dim * block_dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(submatrix_send_b, block_dim * block_dim, MPI_DOUBLE, submatrix_a, block_dim * block_dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if(proc_id == 0) {
        free(submatrix_send_a);
        free(submatrix_send_b);
    }
    int p = sqrt(num_procs);
    MPI_Status *mpi_status;



//    //Now that the blocks have been distributed, we need skew the proc's blocks
    int skew_a_source_col;
    // Need to calculate the wrap around on a negative
    if((proc_coord[1] - proc_coord[0]) % p < 0){
        skew_a_source_col = p + ((proc_coord[1] - proc_coord[0]) % p);
    } else {
        skew_a_source_col = (proc_coord[1] - proc_coord[0]) % p;
    }


    int skew_a_dest = coord_to_index(proc_coord[0], (proc_coord[1] + proc_coord[0]) % p, p);
    int skew_a_source = coord_to_index(proc_coord[0], skew_a_source_col, p);

    int skew_b_source_col;
    if((proc_coord[0] - proc_coord[1]) % p < 0){
        skew_b_source_col = p + ((proc_coord[0] - proc_coord[1]) % p);
    } else {
        skew_b_source_col = (proc_coord[0] - proc_coord[1]) % p;
    }
    int skew_b_dest = coord_to_index((proc_coord[0] + proc_coord[1]) % p, proc_coord[1], p);
    int skew_b_source = coord_to_index(skew_b_source_col, proc_coord[1], p);

    //printf("proc id %d, Skew a source: %d Skew a dest %d\n",proc_id, skew_b_source, skew_b_dest );
//
    MPI_Sendrecv_replace(submatrix_a, block_dim * block_dim, MPI_DOUBLE, skew_a_dest, 0, skew_a_source, 0, MPI_COMM_WORLD, mpi_status);
    MPI_Sendrecv_replace(submatrix_b, block_dim * block_dim, MPI_DOUBLE, skew_b_dest, 0, skew_b_source, 0, MPI_COMM_WORLD, mpi_status);

    // With the skew compete, we can now loop through the blocks sqrt(processes) number of times, multiply the local blocks, and pass the
    // blocks to the next processor
    int matrix_a_dimensions[2] = {block_dim, block_dim};
    int matrix_b_dimensions[2] = {block_dim, block_dim};

    double *submatrix_c;
    submatrix_c = calloc(block_dim * block_dim, sizeof(double));
    for(int stages = 0; stages < p; stages++){

        matrix_matrix_multiply(matrix_a_dimensions, matrix_b_dimensions, submatrix_a, submatrix_b, submatrix_c);


        MPI_Sendrecv_replace(submatrix_a, block_dim * block_dim, MPI_DOUBLE, skew_a_dest, 0, skew_a_source, 0, MPI_COMM_WORLD, mpi_status);
        MPI_Sendrecv_replace(submatrix_b, block_dim * block_dim, MPI_DOUBLE, skew_b_dest, 1, skew_b_source, 1, MPI_COMM_WORLD, mpi_status);
    }

    MPI_Gather(submatrix_c, block_dim*block_dim, MPI_DOUBLE, output_matrix, block_dim*block_dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //Open file to write out
    if(proc_id == 0) {
        int output_dimensions[3] = {block_dim, block_dim, block_dim*block_dim };
        if ((output_file = fopen(argv[2], "w")) == NULL) {
            printf("Failed to open output matrix file\n");
            exit(1);
        }
        write_matrix_to_file(output_file, output_dimensions, output_matrix);
        fclose(output_file);
        free(matrix_a);
        free(matrix_b);
    }


    //Release memory

    free(output_matrix);
    free(submatrix_a);
    free(submatrix_b);
    free(submatrix_c);
    free(proc_coord);

    // Clean up MPI Environment
    MPI_Finalize();
    return(0);
}
