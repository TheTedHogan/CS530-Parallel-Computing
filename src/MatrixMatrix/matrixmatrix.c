#include <stdio.h>
#include <stdlib.h>
#include <mmio.h>
#include "matrixoperations.h"
#include "mpi.h"

#define COORDINATOR 0
#define FROM_COORDINATOR 1
#define FROM_WORKER 2

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

    int numtasks;
    int taskid;
	int numworkers;
	int source;
	int dest;
	int mtype;
    int averow;
    int extra;
    int offset;
    int rows;
    int rc;

    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);

    if (numtasks < 2){
        printf("There needs to be more than 1 task. \n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        exit(1);
    }

    numworkers = numtasks-1;

    if(taskid == COORDINATOR){
        if(argc != 4){
            printf("Usage is: matrixmatrix matrix_a_file matrix_b_file output_file");
            MPI_Abort(MPI_COMM_WORLD, rc);
            exit(1);
        }

        if ((input_matrix_file_a = fopen(argv[1], "r")) == NULL) {
            printf("Failed to open input matrix_a file\n");
            MPI_Abort(MPI_COMM_WORLD, rc);
            exit(1);
        }

        if ((input_matrix_file_b = fopen(argv[2], "r")) == NULL) {
            printf("Failed to open input matrix_b file\n");
            MPI_Abort(MPI_COMM_WORLD, rc);
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
            MPI_Abort(MPI_COMM_WORLD, rc);
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

        averow = matrix_dimensions_a[0]/numworkers;
        extra = matrix_dimensions_a[0]%numworkers;
        offset = 0;
        mtype = FROM_COORDINATOR;
        for (dest=1; dest<=numworkers; dest++)
        {
            rows = (dest <= extra) ? averow+1 : averow;
            printf("Sending %d rows to task %d offset=%d\n",rows,dest,offset);
            MPI_Send(&matrix_dimensions_a, 3, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(&matrix_dimensions_b, 3, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&offset, 1, MPI_INT, dest, 2, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, dest, 3, MPI_COMM_WORLD);
            MPI_Send(&matrix_a[offset+0], rows*matrix_dimensions_a[1], MPI_DOUBLE, dest, mtype,
                    MPI_COMM_WORLD);
            MPI_Send(matrix_b, matrix_dimensions_b[0] * matrix_dimensions_b[1], MPI_DOUBLE, dest, 4, MPI_COMM_WORLD);
            offset = offset + rows;
        }

        /* Receive results from worker tasks */
        mtype = FROM_WORKER;
        for (int i=1; i<=numworkers; i++)
        {
            printf("coordinator is waiting. \n");
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&output_matrix[offset+0], rows*matrix_dimensions_b[1], MPI_DOUBLE, source, mtype,
                    MPI_COMM_WORLD, &status);
            printf("Received results from task %d\n",source);
        }

//         for(int i=0; i < matrix_dimensions_a[0]; i ++){
//             for (int j = 0; j < matrix_dimensions_b[1]; j++){
//                 printf("%g\t", output_matrix[coord_to_index(i, j, matrix_dimensions_b[1])]);
//             }
//             printf("\n");
//         }

        int output_dimensions[3] = {matrix_dimensions_a[0], matrix_dimensions_b[1], matrix_dimensions_a[0] * matrix_dimensions_b[1] };

        //Open file to write out
        if ((output_file = fopen(argv[3], "w")) == NULL) {
            printf("Failed to open output matrix file\n");
            MPI_Abort(MPI_COMM_WORLD, rc);
            exit(1);
        }

        write_matrix_to_file(output_file, output_dimensions, output_matrix);
        fclose(output_file);
        //Release memory
        free(matrix_a);
        free(matrix_b);
    }

    if(taskid > COORDINATOR){
        mtype = FROM_COORDINATOR;
        //printf("Worker %d is trying to receive. \n", taskid);
        MPI_Recv(&matrix_dimensions_a, 3, MPI_INT, COORDINATOR, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        matrix_a = (double *) calloc(matrix_dimensions_a[0] * matrix_dimensions_a[1], sizeof(double));
        MPI_Recv(&matrix_dimensions_b, 3, MPI_INT, COORDINATOR, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        matrix_b = (double *) calloc(matrix_dimensions_b[0] * matrix_dimensions_b[1], sizeof(double));
        output_matrix = (double *) calloc(matrix_dimensions_a[0] * matrix_dimensions_b[1], sizeof(double));
        MPI_Recv(&offset, 1, MPI_INT, COORDINATOR, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, COORDINATOR, 3, MPI_COMM_WORLD, &status);
        MPI_Recv(matrix_a, rows*matrix_dimensions_a[1], MPI_DOUBLE, COORDINATOR, mtype, MPI_COMM_WORLD, &status);

        MPI_Recv(matrix_b, matrix_dimensions_b[0] * matrix_dimensions_b[1], MPI_DOUBLE, COORDINATOR, 4, MPI_COMM_WORLD, &status);
        printf("Worker %d has received data.\n",taskid);

        matrix_matrix_multiply(matrix_dimensions_a, matrix_dimensions_b, matrix_a, matrix_b, output_matrix);


        mtype = FROM_WORKER;
        MPI_Send(&offset, 1, MPI_INT, COORDINATOR, mtype, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, COORDINATOR, mtype, MPI_COMM_WORLD);
        MPI_Send(&output_matrix, rows*matrix_dimensions_b[1], MPI_DOUBLE, COORDINATOR, mtype, MPI_COMM_WORLD);

    }

    MPI_Finalize();
    return 0;

}