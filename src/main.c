#include <stdio.h>
#include <stdlib.h>
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

// argc refers to the number of arguments passed to the program
// argv is an array of character pointers listing all the arguments
int main(int argc, char **argv) {

    const int repeat = 20; // Repeat the calculation to obtain the average flops/s
    const int N_max = atoi(argv[1]); // Second element of our input string, refers to the largest matrix size, required to be 1 to 10 million
    const int N_min = atoi(argv[2]); // Third element, refers to the smallest matrix size in computation
    const int interval = atoi(argv[3]); // atoi converts string to integer

    if (argc != 5) {
        perror("Wrong number of arguments! Please reenter.\n");
        return -1;
    }

    if (N_max < 1 || interval < 1) {
        perror("Matrix size or interval too small!\n");
        return -1;
    }

    double start_time, end_time;
    double total_time[N_max];
    double Gflop[N_max]; // Total number of floating point operations
    double performance[N_max]; // Performance time
    
    // Output N and performance arrays for plotting
    FILE *odata; // Output data

    odata = fopen(argv[4], "w"); // Third element of our input string, refers to output file name
                                // Open file in write mode
    if (odata == NULL) {
        perror("Error in opening file");
        return -1;
    }
    
    fprintf(odata, "%s, %s\n", "matrix size", "GFLOPS/s"); // Print header

    for (int N = N_min; N <= N_max; N += interval) {
        
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
        matC.rows = N;
        matC.cols = N;
        block_matrix_alloc(&matC);
        fill_constant_block_matrix(&matC, 0.0);
        
        // repeat matrix multiplication and measure the total computation time
        get_walltime(&start_time);
        
        for (int i = 0; i < repeat; i++) {
            block_matrix_multiply(matA, matB, &matC);
        }
        
        get_walltime(&end_time);

        //print_matrix(matC);
        
        block_matrix_free(&matA);
        block_matrix_free(&matB);
        block_matrix_free(&matC);
        
        // Performance calculation, GFLOPS/s
        total_time[N-1] = (end_time - start_time) / repeat; // Average time of each repart
        Gflop[N-1] = (matA.rows * matB.cols * matA.cols +
                      matA.rows * matB.cols * (matA.cols - 1)) / 1000000000.0; // Change unit to GFLOPS
        performance[N-1] = Gflop[N-1] / total_time[N-1]; // Compute GFLOPS/s
        
        fprintf(odata, "%d, %f\n", N, performance[N-1]);
    }

    fclose(odata);
    
    return 0;
}
