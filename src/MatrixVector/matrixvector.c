#include <stdio.h>
#include <stdlib.h>


int main() {
    int m, n;
    // Set matrix dimensions, vector size will be determined by this
    m = 5;
    n = 5;
    int matrix[m][n];
    int vector[n];
    int results[m];
    /* Intializes random number generator */
    srand(42);


    /* Create a matrix of the given dimension and fill with random numbers */
    /* TODO: Create matrix from file input */
    for(int i = 0 ; i < m ; i++ ) {
        for(int j = 0; j < n; j++){
            matrix[i][j] = rand() % 10;
            vector[j] = rand() % 10;
        }
    }

    for(int i = 0; i < m; i++){
        results[i] = 0;
        for(int j = 0; j < n; j++) {
            results[i] += matrix[i][j] * vector[j];
        }
    }

    for(int i = 0; i < m; i++){
        printf("%d \n", results[i]);
    }

    return(0);
}
