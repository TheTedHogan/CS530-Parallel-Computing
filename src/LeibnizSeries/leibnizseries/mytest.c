#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


int main(void){
    
    if(system("./myprog -1\n") == 1){
        printf("Input is -1\n");
        printf("FAILED: didn't catch negative value!\n");
    }
    else{
        printf("Input is -1\n");
        printf("SUCCESS: caught negative value!\n");
    }

    if(system("./myprog -0.1\n") == 1){
        printf("Input is -0.1\n");
        printf("FAILED: didn't catch negative decimal value!\n");
    }
    else{
        printf("Input is -0.1\n");
        printf("SUCCESS: caught negative decimal value!\n");
    }

    if(system("./myprog 0\n") == 1){
        printf("Input is 0\n");
        printf("FAILED: didn't catch 0 value!\n");
    }
    else{
        printf("Input is 0\n");
        printf("SUCCESS: caught 0 value!\n");
    }

    if(system("./myprog 0.1\n") == 1){
        printf("Input is 0.1\n");
        printf("FAILED: didn't catch decimal value!\n");
    }
    else{
        printf("Input is 0.1\n");
        printf("SUCCESS: caught decimal value!\n");
    }

    if(system("./myprog 10.1\n") == 1){
        printf("Input is 0.1\n");
        printf("FAILED: didn't catch float value!\n");
    }
    else{
        printf("Input is 10.1\n");
        printf("SUCCESS: caught float value!\n");
    }

    if(system("./myprog\n") == 1){
        printf("Input is NULL\n");
        printf("FAILED: didn't catch NULL value!\n");
    }
    else{
        printf("Input is NULL\n");
        printf("SUCCESS: caught NULL value!\n");
    }
    return 0;
}