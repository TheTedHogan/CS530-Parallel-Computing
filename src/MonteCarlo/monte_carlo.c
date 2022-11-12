#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
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
    #pragma omp parallel default(none) shared(n, inside, entropySource, rand_max, numThreads) private(r,x,y)
    {
        int myseed;
        read(entropySource, &myseed , sizeof(rand));

        #pragma omp single
        {
            numThreads = omp_get_num_threads();
            //printf("Number of threads: %d\n", omp_get_num_threads());
        }
        #pragma omp for reduction(+:inside)
        for (int i = 0; i < n; ++i) {
            x = rand_r(&myseed) / rand_max;
            y = rand_r(&myseed) / rand_max;
            //printf("x is: %f and y is: %f\n", x,y);
            r = sqrt(pow(x, 2) + pow(y, 2));
            //printf("r is equal to: %f\n", r);

            if (r <= 1) {
                inside++;
            }
        }
    }
    close(entropySource);
    clock_t end = clock();
    // printf("inside is equal to: %d\n", inside);
    // inside= 10;
    // pi = 4*(((double)inside)/n);

    // printf("The approximated value of pi is: %.6f\n", pi);
    double timeElapsed = (double)(end - begin)/CLOCKS_PER_SEC/100000.0;

    printf("%d\t%0.6f\t\n", numThreads, timeElapsed);
    return 0;

}
