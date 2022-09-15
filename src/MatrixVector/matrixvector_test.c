#include <stdio.h>
#include <stdlib.h>
#include "matrixoperations.h"


int mm_multiply_test(){
    //create a variable to hold if there was an error encountered
    int error = 0;
    printf("testing matrix_vector_multiply\n");
    printf("testing correct results\n");
    //create a matrix and two vector, the two to be multiplied and the expected results and their dimension
    //matrix_a will be a 2 by 4 matrix
    double matrix[8] = {1, 2, 3, 4,
                          5, 6, 7, 8};
    int matrix_dimensions[3] = {2, 4, 8};

    //the test vector will be a 4 by 1
    double vector[12] = {2, 3, 3,4};
    int vector_dimensions[3] = {4, 1, 4};

    //matrix expected_results will be a 2 by 1 matrix/vector
    double expected_results[6] = {33, 81};
    int expected_result_dimensions[3] = {2, 1, 2};

    //create a vector pointer to collect the actual results
    double *output_vector;
    output_vector = calloc(matrix_dimensions[0], sizeof(double));

    // multiply matrix and vector
    matrix_vector_multiply(matrix_dimensions, vector_dimensions, matrix, vector, output_vector);

    // compare the actual results to the expected results

    for(int i = 0; i < expected_result_dimensions[2]; i++){
        if(expected_results[i] != output_vector[i]){
            error = 1;
        }
    }

    if(error){
        printf("The expected and actual values do not match\n");
        printf("The actual results are:\n");
        for(int i = 0; i < expected_result_dimensions[0]; i++){
            for(int j = 0; j < expected_result_dimensions[1]; j++){
                printf("%g\t", output_vector[coord_to_index(i, j, expected_result_dimensions[1])]);
            }
            printf("\n");
        }
    } else {
        printf("Success! Matrix Vector Multiplication has the expected result\n");
    }

    printf("Testing to ensure there is an error when the matrix and vector are not compatible\n");
    if(matrix_vector_multiply(vector_dimensions, matrix_dimensions, matrix, vector, output_vector)){
        printf("Success! Incompatible shapes return an error value\n");
    } else {
        printf("Error, incompatible matrices did not produce an error value\n");
        error = 1;
    }

    free(output_vector);
    return(error);

}

int main(){
    int any_error = 0;
    if(mm_multiply_test()){
        any_error = 1;
    };
    return(any_error);
}
