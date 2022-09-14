#include <stdio.h>
#include <math.h>
#include <stdlib.h>

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
    for(i = 0; i < n; ++i){
        pi += (pow(-1,i))/(2*i+1);
    }

    pi *= 4;

    printf("The approximated value of pi is: %.6f\n", pi);

    return pi;

}
