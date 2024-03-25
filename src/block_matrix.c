#include <omp.h>
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
			matrix_set(c, i, j, sum);
		}
	}
}

void block_matrix_multiply_parallel(block_matrix_t a, block_matrix_t b, block_matrix_t *c) {
    // Move the print statement outside of the for loop but still within the parallel region
    #pragma omp parallel shared(a, b, c) default(none)
    {
        #pragma omp single
        {
            printf("Number of threads in parallel region: %d\n", omp_get_num_threads());
        }

        #pragma omp for
        for (int i = 0; i < a.rows; i++) {
            for (int j = 0; j < b.cols; j++) {
                float sum = 0.0;
                for (int k = 0; k < a.cols; k++) {
                    sum += matrix_get(a, i, k) * matrix_get(b, k, j);
                }
                matrix_set(c, i, j, sum);
            }
        }
    }
}

void write_matrix_to_csv(const char* filename, block_matrix_t mat) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file to write matrix");
        return;
    }
    
    for (size_t i = 0; i < mat.rows; i++) {
        for (size_t j = 0; j < mat.cols; j++) {
            fprintf(file, "%f", matrix_get(mat, i, j));
            if (j < mat.cols - 1) {
                fprintf(file, ",");  // comma-separated
            }
        }
        fprintf(file, "\n");  // new line for each row
    }
    
    fclose(file);
}