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


    numworkers = numtasks-1;

    if(taskid == COORDINATOR){

        if (numtasks < 2){
            printf("There needs to be more than 1 task. \n");
            MPI_Abort(MPI_COMM_WORLD, rc);
            exit(1);
        }

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

        //printf("worker %d dims %d %d %d \n",taskid, matrix_dimensions_a[0],matrix_dimensions_a[1],matrix_dimensions_a[2]);
        //printf("worker %d dims %d %d %d \n",taskid, matrix_dimensions_b[0],matrix_dimensions_b[1],matrix_dimensions_b[2]);
        for (dest=1; dest<=numworkers; dest++)
        {
            rows = (dest <= extra) ? averow+1 : averow;   	
            //printf("Sending %d rows to task %d offset=%d\n",rows,dest,offset);
        
            MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);

            MPI_Send(&matrix_dimensions_a, 3, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(&matrix_dimensions_b, 3, MPI_INT, dest, 1, MPI_COMM_WORLD);
            
            MPI_Send(&matrix_a[coord_to_index(offset, 0, matrix_dimensions_a[1])], ((rows)*(matrix_dimensions_a[1])), MPI_DOUBLE, dest, mtype,
                    MPI_COMM_WORLD);
            //printf("size matrix a = %lf and size of rows*matrix_dimensions_a[1] is %d\n",matrix_a[coord_to_index(offset, rows, matrix_dimensions_a[1])],rows*matrix_dimensions_a[1]);
            MPI_Send(matrix_b, ((matrix_dimensions_b[0])*(matrix_dimensions_b[1])), MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
            //printf("made it through matrix b\n");
            offset = offset + rows;
        }

        /* Receive results from worker tasks */
        mtype = FROM_WORKER;
        for (int i=1; i<=numworkers; i++)
        {
            //printf("coordinator is waiting. \n");
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&output_matrix[coord_to_index(offset, 0, matrix_dimensions_b[1])], (double)(rows*matrix_dimensions_b[1]), MPI_DOUBLE, source, mtype, 
                    MPI_COMM_WORLD, &status);
            //printf("Received results from task %d\n",source);
        } 

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
        // double matrix_b[matrix_dimensions_b[0] * matrix_dimensions_b[1] * sizeof(double)];
        // double output_matrix[matrix_dimensions_a[0] * matrix_dimensions_b[1] * sizeof(double)];
        // double matrix_a[matrix_dimensions_a[0] * matrix_dimensions_a[1] * sizeof(double)]; 

        //printf("Worker %d is trying to receive. \n", taskid);
        MPI_Recv(&offset, 1, MPI_INT, COORDINATOR, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, COORDINATOR, mtype, MPI_COMM_WORLD, &status);

        //printf("Worker %d is trying to receive rows.... rows = %d \n", taskid,rows);
        
        MPI_Recv(&matrix_dimensions_a, 3, MPI_INT, COORDINATOR, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&matrix_dimensions_b, 3, MPI_INT, COORDINATOR, 1, MPI_COMM_WORLD, &status);
        //printf("Worker %d has received matix dimensions. [%d] = %d  [%d] = %d\n", taskid,0,matrix_dimensions_a[0],1,matrix_dimensions_a[1]);
        
        double* matrix_b;
        matrix_b = calloc(matrix_dimensions_b[0] * matrix_dimensions_b[1], sizeof(double));
        double* sub_matrix_a;
        sub_matrix_a = calloc(rows * matrix_dimensions_a[1], sizeof(double));
        double* sub_output_matrix;
        sub_output_matrix = calloc(rows * matrix_dimensions_b[1], sizeof(double));


        MPI_Recv(sub_matrix_a, ((rows)*(matrix_dimensions_a[1])), MPI_DOUBLE, COORDINATOR, mtype, MPI_COMM_WORLD, &status);
        for(int i = 0; i<(rows)*(matrix_dimensions_a[1]); i++){
            printf("Worker %d is trying to receive matrix a. matrix a = %lf \n", taskid,sub_matrix_a[i]);
        }

        //printf("worker %d received matrix a from 0. Message source = %d, ""tag = %d\n",taskid, status.MPI_SOURCE, status.MPI_TAG);

        MPI_Recv(matrix_b, ((matrix_dimensions_b[0])*(matrix_dimensions_b[1])), MPI_DOUBLE, COORDINATOR, mtype, MPI_COMM_WORLD, &status);
        //printf("Worker %d is trying to receive matrix b. \n", taskid);
        //printf("Worker %d has received data.\n",taskid);

        matrix_dimensions_a[0] = rows;

        //printf("worker %d dims %d %d %d \n",taskid, matrix_dimensions_a[0],matrix_dimensions_a[1],matrix_dimensions_a[2]);
        int sub_matrix_a_dim[3];
        sub_matrix_a_dim[0] = rows;
        sub_matrix_a_dim[1] = matrix_dimensions_a[1];
        sub_matrix_a_dim[2] = matrix_dimensions_a[2];

        matrix_matrix_multiply(sub_matrix_a_dim, matrix_dimensions_b, sub_matrix_a, matrix_b, sub_output_matrix);
        
        
        //printf("Worker %d has this portion of the array\n", taskid);
        // for(int i = 0; i<rows*matrix_dimensions_b[1]; i++){
        //     printf("output %lf\n", output_matrix[i]);
        // }
        // for(int i = 0; i<rows*matrix_dimensions_b[1]; i++){
        //     printf("matrix a %lf\n", matrix_a[i]);
        // }
        mtype = FROM_WORKER;
        MPI_Send(&offset, 1, MPI_INT, COORDINATOR, mtype, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, COORDINATOR, mtype, MPI_COMM_WORLD);
        MPI_Send(sub_output_matrix, rows*matrix_dimensions_b[1], MPI_DOUBLE, COORDINATOR, mtype, MPI_COMM_WORLD);

    }

    MPI_Finalize();
    return 0;

}