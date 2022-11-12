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
    int n;  //number of desired iterations.
    float check;
    float pi = 0;
    double x = 0;  // x value of a particular coordinate.
    double y = 0;  // y value of a particular coordinate.
    double r = 0;  // distance to the origin.
    double rand_max = (double) RAND_MAX;
    int inside = 0; // count for the number of point with r less than 1.
    int entropySource;
    int numThreads;
    int rank;
    int commSize;

    MPI_INIT(&argc, &argv);
    MPI_Comm_Rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_Size(MP_COMM_SIZE, &commSize);

    if(argc != 2){
        fprintf(stderr, "Usage: please input desired number of iterations.\n");
        exit(1);
    }

    //printf("value of check is %f\n", check);
    n = atoi(argv[1]);

    //printf("The desired # of iterations is: %d\n",n);
    validInput(n,atof(argv[1]));

    entropySource = open("/dev/random",  O_RDONLY);
    clock_t begin = clock();

    int myseed;
    read(entropySource, &myseed , sizeof(rand));


    for (int i = 0; i < n; ++i) {

        if (i % commSize != rank) continue;

        x = rand_r(&myseed) / rand_max;
        y = rand_r(&myseed) / rand_max;
        //printf("x is: %f and y is: %f\n", x,y);
        r = sqrt(pow(x, 2) + pow(y, 2));
        //printf("r is equal to: %f\n", r);

        if (r <= 1) {
            inside++;
        }
    }

    close(entropySource);
    clock_t end = clock();
    // printf("inside is equal to: %d\n", inside);
    // inside= 10;
    // pi = 4*(((double)inside)/n);

    // printf("The approximated value of pi is: %.6f\n", pi);
    double timeElapsed = (double)(end - begin)/CLOCKS_PER_SEC/1000.0;

    printf("%d\t%0.6f\t\n", commSize, timeElapsed);
    return 0;

}
