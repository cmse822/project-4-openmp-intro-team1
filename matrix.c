#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "matrix.h"

void matrix_alloc(matrix_t *mat) {
	mat->data = (float **)malloc(mat->rows * sizeof(float *));
	for (int i = 0; i < mat->rows; i++) {
		mat->data[i] = (float *)malloc(mat->cols * sizeof(float));
	}
}

void matrix_free(matrix_t *mat) {
	for (int i = 0; i < mat->rows; i++) {
		free(mat->data[i]);
	}
	free(mat->data);
}

void fill_constant_matrix(matrix_t *mat, float value) {
	for (int i = 0; i < mat->rows; i++) {
		for (int j = 0; j < mat->cols; j++) {
			mat->data[i][j] = value;
		}
	}
}

bool matrix_multiply(matrix_t a, matrix_t b, matrix_t *c) {

	bool found_error = false;

	if (a.cols != b.rows) {
		perror("A and B have incompatible dimensions.");
		found_error = true;
	}

	if (a.rows != c->rows) {
		perror("A and C have incompatible dimensions.");
		found_error = true;
	}

	if (b.cols != c->cols) {
		perror("B and C have incompatible dimensions.");
		found_error = true;
	}

	if (found_error) return false;

	for (int i = 0; i < a.rows; i++) {
		for (int j = 0; j < b.cols; j++) {
			for (int k = 0; k < a.cols; k++) {
				c->data[i][j] += a.data[i][k] * b.data[k][j];
			}
		}
	}
	return true;
}
