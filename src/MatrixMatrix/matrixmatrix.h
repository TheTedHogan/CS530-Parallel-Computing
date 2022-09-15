//
// Created by Ted Hogan on 9/14/22.
//

#ifndef CS530_PARALLEL_COMPUTING_MATRIXMATRIX_H
#define CS530_PARALLEL_COMPUTING_MATRIXMATRIX_H

int matrix_matrix_multiply(int matrix_dimensions_a[], int matrix_dimensions_b[], double matrix_a[], double matrix_b[], double *output_matrix);
int coord_to_index(int row_coord, int col_coord, int columns);

#endif //CS530_PARALLEL_COMPUTING_MATRIXMATRIX_H
