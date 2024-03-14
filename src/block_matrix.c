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
			for (int k = 0; k < a.cols; k++) {
				matrix_set(c, i, j, matrix_get(a, i, k) * matrix_get(b, k, j));
			}
		}
	}

}
