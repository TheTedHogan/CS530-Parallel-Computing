#include <stdio.h>
#include <stdlib.h>
#include <mmio.h>
#include "matrixoperations.h"
#include <math.h>
#include <mpi.h>



int main(int argc, char *argv[]) {
    int num_procs;
    int proc_id;
    int n;
    MM_typecode matcode;
    FILE *input_matrix_file_a;
    FILE *input_matrix_file_b;
    FILE *output_file;
    int matrix_dimensions_a[3];
    int matrix_dimensions_b[3];
    double *matrix_a;
    double *matrix_b;
    double *output_matrix;

    if(argc != 3){
        printf("Usage is: matrixmatrix n output_file");
    }
    n = argv[1];



    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
    int *proc_coord;
    proc_coord = malloc(2 * sizeof(int));
    index_to_coordinate(proc_id, sqrt(num_procs), proc_coord);


    if(proc_id == 0){
        output_matrix = calloc(matrix_dimensions_a[0] * matrix_dimensions_b[1], sizeof(double));

        if(sqrt(num_procs) - floor(sqrt(num_procs)) != 0){
            printf("Number of processors must be a perfect square\n");
            MPI_Finalize();
            exit(1);
        }

        if(n % (int)sqrt(num_procs)){
            printf("Matrix dimensions are not divisible by %d\n", (int)sqrt(num_procs));
        }

        //allocate memory for first matrix_a
        matrix_a = (double *) calloc(num_procs * num_procs, sizeof(double));
        matrix_b = (double *) calloc(num_procs * num_procs, sizeof(double));
        random_square_matrix(n, matrix_a);
        random_square_matrix(n, matrix_b);

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
        free(submatrix_send_a);
        free(submatrix_send_b);
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
    MPI_Sendrecv_replace(submatrix_b, block_dim * block_dim, MPI_DOUBLE, skew_b_dest, 1, skew_b_source, 1, MPI_COMM_WORLD, mpi_status);

    // With the skew compete, we can now loop through the blocks sqrt(processes) number of times, multiply the local blocks, and pass the
    // blocks to the next processor
    int *submatrix_c;
    submatrix_c = calloc(block_dim * block_dim, sizeof(double));
    for(int stages = 0; stages < p; stages++){
        for(int i = 0; i < block_dim; i++){
            for(int j = 0; j < block_dim; j++){
                submatrix_c[coord_to_index(i, j, block_dim)] = 0;
                for(int k; k < block_dim; k++){
                    submatrix_c[coord_to_index(i, j, block_dim)] += submatrix_a[coord_to_index(i, k, block_dim)] * submatrix_b[coord_to_index(k, j, block_dim)];
                }
            }
        }
        MPI_Sendrecv_replace(submatrix_a, block_dim * block_dim, MPI_DOUBLE, skew_a_dest, 0, skew_a_source, 0, MPI_COMM_WORLD, mpi_status);
        MPI_Sendrecv_replace(submatrix_b, block_dim * block_dim, MPI_DOUBLE, skew_b_dest, 1, skew_b_source, 1, MPI_COMM_WORLD, mpi_status);
    }

    MPI_Gather(submatrix_c, block_dim*block_dim, MPI_DOUBLE, output_matrix, block_dim*block_dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);

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




    int output_dimensions[3] = {block_dim, block_dim, block_dim*block_dim };

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
    free(output_matrix);
    free(submatrix_a);
    free(submatrix_b);
    free(submatrix_c);
    free(proc_coord);

    // Clean up MPI Environment
    MPI_Finalize();
    return(0);
}
