#include <stdio.h>
#include <stdlib.h>
#include "matrixoperations.h"


int mm_multiply_test(){
    //create a variable to hold if there was an error encountered
    int error = 0;
    printf("testing matrix_matrix_multiply\n");
    printf("testing correct results\n");
    //create three matrices, the two to be multiplied and the expected results and their dimension
    //matrix_a will be a 2 by 4 matrix
    double matrix_a[8] = {1, 2, 3, 4,
                          5, 6, 7, 8};
    int matrix_a_dimensions[3] = {2, 4, 8};
    //matrix_b will be a 4 by 3 matrix
    double matrix_b[12] = {2, 3, 3,
                           2,3, 4,
                           2, 5, 7,
                           1, 5, 4,};

    int matrix_b_dimensions[3] = {4, 3, 12};
    //matrix expected_results will be a 2 by 3 matrix
    double expected_results[6] = {16, 44, 48,
                                  44, 108, 120};
    int expected_result_dimensions[3] = {2, 3, 6};
    //create a matrix pointer to collect the actual results
    double *output_matrix;
    output_matrix = calloc(matrix_a_dimensions[0] * matrix_b_dimensions[1], sizeof(double));
    // multiply matrix_a and matrix_b
    matrix_matrix_multiply(matrix_a_dimensions, matrix_b_dimensions, matrix_a, matrix_b, output_matrix);

    // compare the actual results to the expected results

    for(int i = 0; i < expected_result_dimensions[2]; i++){
        if(expected_results[i] != output_matrix[i]){
            error = 1;
        }
    }

    if(error){
        printf("The expected and actual values do not match\n");
        printf("The actual results are:\n");
        for(int i = 0; i < expected_result_dimensions[0]; i++){
            for(int j = 0; j < expected_result_dimensions[1]; j++){
                printf("%g\t", output_matrix[coord_to_index(i, j, expected_result_dimensions[1])]);
            }
            printf("\n");
        }
    } else {
        printf("Success! MatrixMatrix_Multiplication has the expected result\n");
    }

    printf("Testing to ensure there is an error when the matrixes are not compatible\n");
    if(matrix_matrix_multiply(matrix_b_dimensions, matrix_a_dimensions, matrix_b, matrix_a, output_matrix)){
        printf("Success! Incompatible matrices return an error value\n");
    } else {
        printf("Error, incompatible matrices did not produce an error value\n");
        error = 1;
    }

    free(output_matrix);
    return(error);

}

int main(){
    int any_error = 0;
    if(mm_multiply_test()){
        any_error = 1;
    };
    return(any_error);
}
