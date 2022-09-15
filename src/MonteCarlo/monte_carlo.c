#include <stdio.h>
#include <math.h>
#include <stdlib.h>
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
    float x;  // x value of a particular coordinate.
    float y;  // x value of a particular coordinate.
    float r;  // distance to the origin.
    int inside = 0; // count for the number of point with r less than 1.

    if(argc != 2){
        fprintf(stderr, "Usage: please input desired number of iterations.\n");
        exit(1);
    }

    check = atof(argv[1]);
    //printf("value of check is %f\n", check);

    n = atoi(argv[1]);
    //printf("The desired # of iterations is: %d\n",n);

    validInput(n,check);
    

    srand(time(0));
    //printf("random # is: %d\n", rand());
    //printf("rand max is: %d\n", RAND_MAX);

    int i;
    for(i = 0; i < n; ++i){
        x = (rand()/((double)RAND_MAX));
        y = (rand()/((double)RAND_MAX));
        //printf("x is: %f and y is: %f\n", x,y);

        r = sqrtf(pow(x,2)+pow(y,2));
        //printf("r is equal to: %f\n", r);

        if(r <= 1){
            inside++;
        }
    }
    //printf("inside is equal to: %d\n", inside);
    pi = 4*(((double)inside)/n);

    printf("The approximated value of pi is: %.6f\n", pi);

    return 0;

}
