#include <stdlib.h>
#include <stdio.h>
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

void block_matrix_multiply(block_matrix_t a, block_matrix_t b, block_matrix_t *c) {
	for (int i = 0; i < a.rows; i++) {
		for (int j = 0; j < b.cols; j++) {
			float sum = 0.0;
			for (int k = 0; k < a.cols; k++) {
				sum += matrix_get(a, i, k) * matrix_get(b, k, j);
			}
			matrix_set(c, i, j, sum); // this is the dot product
		}
	}
}

void block_matrix_multiply_parallel(block_matrix_t a, block_matrix_t b, block_matrix_t *c) {
	#pragma omp parallel shared(a, b, c) {
		#pragma omp for
		for (int i = 0; i < a.rows; i++) {
			#pragma omp for
			for (int j = 0; j < b.cols; j++) {
				float sum = 0.0;
				#pragma omp for reduction(+:sum)
				for (int k = 0; k < a.cols; k++) {
					sum += matrix_get(a, i, k) * matrix_get(b, k, j);
				}
				matrix_set(c, i, j, sum);
			}
		}
	}
