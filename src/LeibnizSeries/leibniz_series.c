#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

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
    int n;  //number of desired iterations.
    float check;
    int rank;
    int commSize;

    MPI_INIT(&argc, &argv);
    MPI_Comm_Rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_Size(MPI_COMM_SIZE, &commSize);

    MPI_BARRIER(MPI_COMM_WORLD);

    if(argc != 2){
        fprintf(stderr, "Usage: please input desired number of iterations.\n");
        exit(1);
    }

    check = atof(argv[1]);
    //printf("value of check is %f\n", check);

    n = atoi(argv[1]);
    //printf("The desired # of iterations is: %d\n",n);

    validInput(n,check);

    double startTime = MPI_Wtime();

    MPI_BCast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int count = n / commSize;
    int start = count * rank;
    int end = count * rank + count;

    int i = 0;
    double pi = 0;
    double segmentTotal = 0;

    for (i = start; i < end; i++) {
      segmentTotal += pow(-1, i)/(2 * i + 1);
    }

    MPI_REDUCE(&segmentTotal, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double endTime = MPI_Wtime();
    //printf("The approximated value of pi is: %.6f\n", pi);
    if (rank == 0) {
      total = total *= 4;
      printf("%2d\t%fsecs\t%0.6f", size, startTime - endTime, pi)
    }

    MPI_Finalize();
    return pi;

}
