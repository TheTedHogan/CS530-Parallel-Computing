#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <fcntl.h>

void validInput(int input_1, float input_2){
    if(input_1 < 1){
        fprintf(stderr, "input must be and integer greater or equal to 1.\n");
        exit(1);
    }
    if(input_2/((float)input_1) != 1){
        fprintf(stderr, "input must be and integer greater or equal to 1.\n");
        exit(1);
    }
}

int main(int argc, char * argv[]){
    int rank;
    int size;
    int error;
    int i;
    int result = 0;
    int sum = 0;
    double pi = 0;
    double start;
    double end;
    double x;
    double y;
    int n;

    error = MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();

    if(argc != 2){
        fprintf(stderr, "Usage: please input desired number of iterations.\n");
        exit(1);
    }

    //printf("value of check is %f\n", check);
    n = atoi(argv[1]);

    //printf("The desired # of iterations is: %d\n",n);
    validInput(n,atof(argv[1]));

    srand((int)time(0));


    for (i = 0; i < n; i+=size) {
      x = rand()/(RAND_MAX+1.0);
      y = rand()/(RAND_MAX+1.0);
      if (x*x+y*y < 1) {
        result++;
      }
    }

    MPI_Reduce(&result, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    if (rank == 0) {
      pi = 4*((double)sum/n);
        printf("Elapsed Time\t%0.5f\n", end - start);
    }

    MPI_Finalize();

    return 0;

}
