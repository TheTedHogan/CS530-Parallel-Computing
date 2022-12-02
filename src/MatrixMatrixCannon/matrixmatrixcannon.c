#include <stdio.h>
#include <stdlib.h>
#include <mmio.h>
#include "matrixoperations.h"
#include <math.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char *argv[]) {
    MPI_Status *mpi_status;
    FILE *output_file;
    int output_dimensions[3];
    int num_procs;
    int proc_id;
    int n;
    int p;
    int block_dim;
    int skew_a_source_col = 0;
    int skew_a_dest = 0;
    int skew_a_source = 0;
    int skew_b_source_col = 0;
    int skew_b_dest = 0;
    int skew_b_source = 0;
    int shift_a_source_col =0;
    int shift_a_dest=0;
    int shift_a_source=0;
    int shift_b_source_col =0;
    int shift_b_dest=0;
    int shift_b_source=0;
    int coord_start[2];
    int coord_end[2];
    int* proc_coord;
    double* matrix_a;
    double* matrix_b;
    double* matrix_c;
    double* output_matrix;
    double* submatrix_a;
    double* submatrix_b;
    double *submatrix_c;
    double* submatrix_send_a;
    double* submatrix_send_b;

    n = atoi(argv[1]);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    p = (int)sqrt(num_procs);


    block_dim = n / p;
    proc_coord = malloc(2 * sizeof(int));
    index_to_coordinate(proc_id, sqrt(num_procs), proc_coord);

    submatrix_a = calloc(block_dim * block_dim, sizeof(double));
    submatrix_b = calloc(block_dim * block_dim, sizeof(double));
    submatrix_c = calloc(block_dim * block_dim, sizeof(double));

    //initialize the submatrix_c to 0 to prep it for collecting results
    for(int i = 0; i < block_dim * block_dim; i++){
        submatrix_c[i] = 0;
    }


    if(proc_id == 0){
        if(sqrt(num_procs) - floor(sqrt(num_procs)) != 0){
            printf("Number of processors must be a perfect square\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            exit(1);
        }
        if(n % (int)sqrt(num_procs)){
            printf("Matrix dimensions: %d, are not divisible by %d\n", n, (int)sqrt(num_procs));
            MPI_Abort(MPI_COMM_WORLD, 1);
            exit(1);
        }
        matrix_a = (double *) calloc(n * n, sizeof(double));
        matrix_b = (double *) calloc(n * n, sizeof(double));
        matrix_c = (double *) calloc(n * n, sizeof(double));
        output_matrix = (double *) calloc(n * n, sizeof(double));
        submatrix_send_a = calloc(n * n, sizeof(double));
        submatrix_send_b = calloc(n * n, sizeof(double));
//        random_square_matrix(n, matrix_a);
//        random_square_matrix(n, matrix_b);
        //initialize the output matrix to 0
        for(int i = 0; i < n*n; i++){
            matrix_a[i] = i;
            matrix_b[i] = i;
            output_matrix[i] = 0;
        }

        for(int x = 0; x < num_procs; x++){
            int* dest_coord;
            dest_coord = malloc(2 * sizeof(int));
            index_to_coordinate(x, sqrt(num_procs), dest_coord);
            coord_start[0] = dest_coord[0]* block_dim;
            coord_start[1] = dest_coord[1]* block_dim;
            coord_end[0] = coord_start[0] + block_dim;
            coord_end[1] = coord_start[1] + block_dim;

            for(int i = 0; i < block_dim; i++) {
                for (int j = 0; j < block_dim; j++) {

                    submatrix_send_a[(x*block_dim*block_dim)+(i*block_dim)+j] = matrix_a[coord_to_index(coord_start[0]+i, coord_start[1]+j, n)];
                    submatrix_send_b[(x*block_dim*block_dim)+(i*block_dim)+j] = matrix_b[coord_to_index(coord_start[0]+i, coord_start[1]+j, n)];

                }
            }
            free(dest_coord);
        }
    }

    //Scatter the intial submatricies to each of the processes
    MPI_Scatter(submatrix_send_a, block_dim * block_dim, MPI_DOUBLE, submatrix_a, block_dim * block_dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(submatrix_send_b, block_dim * block_dim, MPI_DOUBLE, submatrix_b, block_dim * block_dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //Calculate the skew for A
    if((proc_coord[1] - proc_coord[0]) %  p < 0){
        skew_a_source_col =  p + ((proc_coord[1] - proc_coord[0]) %  p);
    } else {
        skew_a_source_col = (proc_coord[1] - proc_coord[0]) %  p;
    }
    skew_a_source = coord_to_index(proc_coord[0], (proc_coord[1] + proc_coord[0]) %  p,  p);
    skew_a_dest = coord_to_index(proc_coord[0], skew_a_source_col,  p);


    //calculate the skew for B
    if((proc_coord[0] - proc_coord[1]) %  p < 0){
        skew_b_source_col =  p + ((proc_coord[0] - proc_coord[1]) %  p);
    } else {
        skew_b_source_col = (proc_coord[0] - proc_coord[1]) %  p;
    }
    skew_b_source = coord_to_index((proc_coord[0] + proc_coord[1]) %  p, proc_coord[1],  p);
    skew_b_dest = coord_to_index(skew_b_source_col, proc_coord[1],  p);

    // Perform the initial alignment
    MPI_Sendrecv_replace(submatrix_a, block_dim * block_dim, MPI_DOUBLE, skew_a_dest, 0, skew_a_source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Sendrecv_replace(submatrix_b, block_dim * block_dim, MPI_DOUBLE, skew_b_dest, 0, skew_b_source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //printf("proc\t%d\tdest\t%d\tsource\t%d\n",proc_id, skew_b_dest, skew_b_source);
    //Calculate the shift for A
    if((proc_coord[1] - 1) %  p < 0){
        shift_a_source_col =  p + ((proc_coord[1] - 1) %  p);
    } else {
        shift_a_source_col = (proc_coord[1] - 1) %  p;
    }
    shift_a_source = coord_to_index(proc_coord[0], (proc_coord[1]+1) %  p,  p);
    shift_a_dest = coord_to_index(proc_coord[0], shift_a_source_col,  p);

    //Calculate the shift for B
    if((proc_coord[0] - 1) %  p < 0){
        shift_b_source_col =  p + ((proc_coord[0] - 1) %  p);
    } else {
        shift_b_source_col = (proc_coord[0] - 1) %  p;
    }
    shift_b_source = coord_to_index((proc_coord[0]+1) %  p, proc_coord[1],  p);
    shift_b_dest = coord_to_index(shift_b_source_col, proc_coord[1],  p);

    int stage, i,j,k;
    for(stage= 0; stage <  p; stage++) {
        for(i = 0; i < block_dim; i++){
            for(j = 0; j < block_dim; j++){
                for(k = 0; k < block_dim; k++){
                    submatrix_c[coord_to_index(i, j, block_dim)] += submatrix_a[i*block_dim+k] * submatrix_b[k*block_dim+j];
                }
            }
        }

        MPI_Sendrecv_replace(submatrix_a, block_dim * block_dim, MPI_DOUBLE, shift_a_dest, 0, shift_a_source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Sendrecv_replace(submatrix_b, block_dim * block_dim, MPI_DOUBLE, shift_b_dest, 0, shift_b_source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }

    //Gather back the finished submatrix_c's
    MPI_Gather(submatrix_c, block_dim*block_dim, MPI_DOUBLE, matrix_c, block_dim*block_dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Take the individual result matrices and reorder them into the original form
    if(proc_id == 0) {
        for(int x = 0; x < num_procs; x++){
            int* dest_coord;
            dest_coord = malloc(2 * sizeof(int));
            index_to_coordinate(x, sqrt(num_procs), dest_coord);
            coord_start[0] = dest_coord[0]* block_dim;
            coord_start[1] = dest_coord[1]* block_dim;
            coord_end[0] = coord_start[0] + block_dim;
            coord_end[1] = coord_start[1] + block_dim;
            for(int i = 0; i < block_dim; i++){
                for(int j = 0; j < block_dim; j++){
                    output_matrix[coord_to_index(coord_start[0]+i, coord_start[1]+j, n)] = matrix_c[(x*block_dim*block_dim)+(i*block_dim)+j];
                }
            }
            free(dest_coord);
        }
//        for(int z = 0; z < n * n; z ++){
//            if(z % n == 0){
//                printf("\n");
//            }
//            printf("%g\t", output_matrix[z]);
//        }
//
//        printf("\n");
        output_dimensions[0] = block_dim;
        output_dimensions[1] = block_dim;
        output_dimensions[2] = block_dim * block_dim;
        printf("filename %s\n",argv[2]);
        if ((output_file = fopen(argv[2], "w")) == NULL) {
            printf("Failed to open output matrix file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            exit(1);
        }

        write_matrix_to_file(output_file, output_dimensions, output_matrix);
        fclose(output_file);

    }


    free(proc_coord);
    free(submatrix_a);
    free(submatrix_b);
    free(submatrix_c);
    free(matrix_a);
    free(matrix_b);
    free(matrix_c);
    MPI_Finalize();


    return 0;
}

