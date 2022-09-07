#include <stdio.h>
#include <stdlib.h>

int main() {
    int m, n;
    // Set matrix dimensions, both matrices are assumed square matrix
    m = 5;
    int matrix_a[m][m];
    int matrix_b[m][m];
    int results[m][m];

    /* Intializes random number generator */
    srand(42);


    /* Create two matrices of the given dimension and fill with random numbers */
    /* TODO: Create matrix from file input */
    for(int i = 0 ; i < m ; i++ ) {
        for(int j = 0; j < n; j++){
            matrix_a[i][j] = rand() % 10;
            matrix_b[i][j] = rand() % 10;
        }
    }

    for(int i = 0; i < m; i++){
        for(int j = 0; j < m; j++) {
            results[i][j] += matrix_a[i][j] * matrix_b[i][j];
        }
    }

    for(int i = 0; i < m; i++){
        for(int j=0; j<m; j++){
            printf("%d \t", results[i][j]);
        }
        printf("\n");
    }

    return(0);
}
