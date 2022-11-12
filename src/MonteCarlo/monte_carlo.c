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

    error = MPI_INIT(&argc, &argv);
    MPI_Comm_Rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_Size(MP_COMM_SIZE, &commSize);
    MPI_BARRIER(MPI_COMM_WORLD);

    start = MPI_WTIME();

    if(argc != 2){
        fprintf(stderr, "Usage: please input desired number of iterations.\n");
        exit(1);
    }

    //printf("value of check is %f\n", check);
    n = atoi(argv[1]);

    //printf("The desired # of iterations is: %d\n",n);
    validInput(n,atof(argv[1]));

    srand((int)time());


    for (i = rank; i < 1E8; i+=size) {
      x = rand()/(RAND_MAX+1.0);
      y = rand()/(RAND_MAX+1.0);
      if (x*x+y*y < 1) {
        result++;
      }
    }

    MPI_REDUCE(&result, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_BARRIER(MPI_COMM_WORLD);
    end = MPI_WTIME();

    if (rank == 0) {
      pi = 4*1E-8*sum;
      printf("%2d\t%fsecs\t%0.6f\t", size, start - end, pi);
    }

    return 0;

}
