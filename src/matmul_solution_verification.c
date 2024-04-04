#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "../include/matrix.h"
#include "../include/block_matrix.h"
#include "../include/get_walltime.h"

void print_matrix(block_matrix_t mat) {
	for (int i = 0; i < mat.rows; i++) {
		for(int j = 0; j < mat.cols; j++) {
			printf("%f ", matrix_get(mat, i, j));
		}
		printf("\n");
	}
}

// compare whether mat1 is equal to mat2
int compare_matrices(block_matrix_t mat1, block_matrix_t mat2) {
    if (mat1.rows != mat2.rows || mat1.cols != mat2.cols) {
        return 0;
    }
    for (int i = 0; i < mat1.rows; i++) {
        for (int j = 0; j < mat1.cols; j++) {
            if (matrix_get(mat1, i, j) != matrix_get(mat2, i, j)) {
                return 0;
            }
        }
    }
    return 1;
}

int main() {

    const int maxThreads = 128; // the max threads of AMD-20
    const int N = 20; // the matirx size
    FILE *file = fopen("matrixResults.txt", "w"); // Output data
        
    block_matrix_t matA;
    matA.rows = N;
    matA.cols = N;
    block_matrix_alloc(&matA);
    fill_constant_block_matrix(&matA, -1.0);
    
    block_matrix_t matB;
    matB.rows = N;
    matB.cols = N;
    block_matrix_alloc(&matB);
    fill_constant_block_matrix(&matB, 2.0);
    
    block_matrix_t matC;

    // matD is the result of serial calculation, 
    // all parallel results will be compared with matD.
    block_matrix_t matD;
    matD.rows = N;
    matD.cols = N;
    block_matrix_alloc(&matD);
    fill_constant_block_matrix(&matD, 0.0);
    block_matrix_multiply(matA, matB, &matD);

    // Will test the MMM results of 1,2,4,8...128 threads
    for (int t = 1; t <= maxThreads; t *= 2) {
        omp_set_num_threads(t);

        matC.rows = N;
        matC.cols = N;
        block_matrix_alloc(&matC);
        fill_constant_block_matrix(&matC, 0.0); 
        
        block_matrix_multiply_parallel(matA, matB, &matC);

        // Compare all elements in matC and matD, then out put the results to matrixResults.txt
        int isEqual = compare_matrices(matC, matD);
        fprintf(file, "Threads: %d, Result is %s\n", t, isEqual ? "Same" : "Different");

        block_matrix_free(&matC);
    }

    block_matrix_free(&matA);
    block_matrix_free(&matB);
    block_matrix_free(&matD);
    fclose(file);
    
    return 0;
}
