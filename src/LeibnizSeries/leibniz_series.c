#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>
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
    float pi;
    int numThreads;

    if(argc != 2){
        fprintf(stderr, "Usage: please input desired number of iterations.\n");
        exit(1);
    }

    check = atof(argv[1]);
    //printf("value of check is %f\n", check);

    n = atoi(argv[1]);
    //printf("The desired # of iterations is: %d\n",n);

    validInput(n,check);


    int i;
    clock_t begin = clock();
    #pragma omp parallel default(none) shared(n, numThreads) reduction(+:pi)
    {
      #pragma omp single
      {
        numThreads = omp_get_num_threads();
      }
      #pragma omp for
      for(i = 0; i < n; ++i){
          pi += (pow(-1,i))/(2*i+1);
      }
    }
    clock_t end = clock();

    pi *= 4;

    double timeSpent = ((double)(end - begin)/CLOCKS_PER_SEC) / 1000;
    //printf("The approximated value of pi is: %.6f\n", pi);
    printf("%d\t%0.6f\t\n", numThreads, timeSpent);
    return pi;

}
