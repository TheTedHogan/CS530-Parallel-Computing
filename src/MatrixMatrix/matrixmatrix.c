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
    double *final_output_matrix;
    double *work_matrix_a;
    double *work_matrix_b;


    int numtasks;
    int taskid;               
	int numworkers;     
    int averows;
    int extra; 
    int rc;  
    int gathercount;

    int sendcount[numworkers];    
    int displ[numworkers];


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

        // Fetch the dimensions for the matrix_b
        get_mmio_dimensions(input_matrix_file_b, matrix_dimensions_b);

        //allocate memory for matrix_b
        matrix_b = (double *) calloc(matrix_dimensions_b[0] * matrix_dimensions_b[1], sizeof(double));

        //Populate the matrix_a
        create_matrix_array(input_matrix_file_b, matrix_dimensions_b, matrix_b);

        
        fclose(input_matrix_file_b);

        output_matrix = calloc(matrix_dimensions_a[0] * matrix_dimensions_b[1], sizeof(double));
        final_output_matrix = calloc(matrix_dimensions_a[0] * matrix_dimensions_b[1], sizeof(double));

        averows = (matrix_dimensions_a[1]*matrix_dimensions_a[0])/numworkers;
        extra = (matrix_dimensions_a[1]*matrix_dimensions_a[0])%numworkers;

        displ[0] = 0;
        for(int i = 0; i < numworkers; i++){
            sendcount[i] = averows;
            //printf("sendcount[%d] = %d\n",i,sendcount[i]);
        }
        for (int i = 1; i < numworkers; i++){
            displ[i] = displ[i-1] + sendcount[i-1];
            //printf("displ[%d] = %d\n",i,displ[i]);
        }
        sendcount[numworkers-1] += extra;
        //printf("sendcount[numworkers-1] = %d\n",sendcount[numworkers-1]);
        displ[numworkers-2] += sendcount[numworkers-1];
        //printf("displ[numworkers-1] = %d\n",displ[numworkers-1]);

        // for(int i=0; i < matrix_dimensions_a[0]; i ++){
        //     for (int j = 0; j < matrix_dimensions_a[1]; j++){
        //         printf("%g\t", matrix_a[coord_to_index(i, j, matrix_dimensions_a[1])]);
        //     }
        //     printf("\n");
        // }
        // for(int i=0; i < matrix_dimensions_b[0]; i ++){
        //     for (int j = 0; j < matrix_dimensions_b[1]; j++){
        //         printf("%g\t", matrix_b[coord_to_index(i, j, matrix_dimensions_b[1])]);
        //     }
        //     printf("\n");
        // }

    }
    averows = (matrix_dimensions_a[1]*matrix_dimensions_a[0])/numworkers;
    extra = (matrix_dimensions_a[1]*matrix_dimensions_a[0])%numworkers;

    double work_matrix_a[averows+extra];
    double work_matrix_b[averows+extra];
    //double work_output_matrix[averows+extra];

    MPI_Scatterv(&matrix_a, sendcount, displ, MPI_INT, &work_matrix_a, averows+extra, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD); 
    //printf("worker %d receieved matrix_a\n", taskid);
    MPI_Scatterv(&matrix_b, sendcount, displ, MPI_INT, &work_matrix_b, averows+extra, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD); 

    //MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(matrix_dimensions_a, 3, MPI_INT, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(matrix_dimensions_b, 3, MPI_INT, COORDINATOR, MPI_COMM_WORLD);

    //MPI_Bcast(&matrix_b, matrix_dimensions_b[0]*matrix_dimensions_b[1], MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    //printf("worker %d receieved matrix_b\n", taskid);
    //MPI_Bcast(&output_matrix, matrix_dimensions_a[0]*matrix_dimensions_b[1], MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    //printf("worker %d receieved output_matrix\n", taskid);   

    if(taskid > COORDINATOR){
        //printf("worker %d\n",taskid);
        for(int i = 0; i<(matrix_dimensions_a[1]*matrix_dimensions_a[0]); i++){
            printf("work_matrix_a[%d] = %d\n", i, work_matrix_a[i]);
        }
        for(int i = 0; i<(matrix_dimensions_a[1]*matrix_dimensions_a[0]); i++){
            //printf("work_matrix_b[%d] = %d\n", i, work_matrix_b[i]);
        }
        matrix_matrix_multiply(matrix_dimensions_a, matrix_dimensions_b, work_matrix_a, work_matrix_b, output_matrix);

        for(int i = 0; i<(matrix_dimensions_a[1]*matrix_dimensions_a[0]); i++){
            //printf("work_output_matrix[%d] = %lf\n", i, work_output_matrix[i]);
        }
    }

    gathercount = (matrix_dimensions_a[0]*matrix_dimensions_b[1])/numtasks;
    MPI_Gatherv(output_matrix, gathercount, MPI_DOUBLE, final_output_matrix, sendcount, displ, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    //MPI_Barrier(MPI_COMM_WORLD);

    if (taskid == COORDINATOR){
        //Open file to write out
        if ((output_file = fopen(argv[3], "w")) == NULL) {
            printf("Failed to open output matrix file\n");
            MPI_Abort(MPI_COMM_WORLD, rc);
            exit(1);
        }
        int output_dimensions[3] = {matrix_dimensions_a[0], matrix_dimensions_b[1], matrix_dimensions_a[0] * matrix_dimensions_b[1] };
        write_matrix_to_file(output_file, output_dimensions, output_matrix);
        fclose(output_file);
        //Release memory
        free(matrix_a);
        free(matrix_b);
    }

    MPI_Finalize();
    return 0;

}