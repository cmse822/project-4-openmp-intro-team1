#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "../include/block_matrix.h"

void block_matrix_alloc(block_matrix_t *mat) {
	mat->data = (float *)malloc(mat->rows * mat->cols * sizeof(float));
}

void block_matrix_free(block_matrix_t *mat) {
	free(mat->data);
}

void fill_constant_block_matrix(block_matrix_t *mat, float value) {
	for (int i = 0; i < mat->rows; i++) {
		for (int j = 0; j < mat->cols; j++) {
			matrix_set(mat, i, j, value);
		}
	}
}

// Assume 'a' is an m x n matrix, 'b' is an n x k matrix, and 'c' is an m x k matrix
void block_matrix_multiply(block_matrix_t a, block_matrix_t b, block_matrix_t *c) {
    size_t m = a.rows;
    size_t n = a.cols; 
    size_t k = b.cols;
    // Single loop to iterate over all elements of the m x k matrix 'c'.
    for (int idx = 0; idx < m * k; idx++) {
        int i = idx / k; // row index for 'c'.
        int j = idx % k; // col index for 'c'.
        float sum = 0.0;

        // Compute the dot product to get the element c[i][j].
        for (int p = 0; p < n; p++) {
            sum += matrix_get(a, i, p) * matrix_get(b, p, j);
        }
        matrix_set(c, i, j, sum);
    }
}

void block_matrix_multiply_parallel(block_matrix_t a, block_matrix_t b, block_matrix_t *c) {
    size_t m = a.rows;
    size_t n = a.cols; 
    size_t k = b.cols;

    // Parallel region begins
    #pragma omp parallel shared(a, b, c) 
    {
        // Parallelize the outermost single loop
        #pragma omp for 
        for (int idx = 0; idx < m * k; idx++) {
            int i = idx / k; // row index for 'c'.
            int j = idx % k; // col index for 'c'.
            float sum = 0.0; // sum is now local to each iteration of the loop.

            // Perform the dot product to compute the element c[i][j].
            for (int p = 0; p < n; p++) {
                sum += matrix_get(a, i, p) * matrix_get(b, p, j);
            }

            // Update the matrix 'c' with the dot product result.
            // Access to 'c' should be synchronized if multiple threads can write to the same location
            matrix_set(c, i, j, sum);
        }
    }
    // Parallel region ends
}


