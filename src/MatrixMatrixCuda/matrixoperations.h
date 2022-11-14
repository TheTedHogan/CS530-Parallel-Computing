//
// Created by Ted Hogan on 9/14/22.
//

#ifndef CS530_PARALLEL_COMPUTING_MATRIXOPERATIONS_H
#define CS530_PARALLEL_COMPUTING_MATRIXOPERATIONS_H

int coord_to_index(int row_coord, int col_coord, int columns);
int matrix_matrix_multiply(int matrix_dimensions_a[], int matrix_dimensions_b[], double matrix_a[], double matrix_b[], double *output_matrix);
void get_mmio_dimensions(FILE *f, int* dimensions_out);
void create_matrix_array(FILE *f, int * dimensions, double *matrix_out);
int write_matrix_to_file(FILE *f, int dimensions[], double matrix_out[]);

#endif //CS530_PARALLEL_COMPUTING_MATRIXOPERATIONS_H
