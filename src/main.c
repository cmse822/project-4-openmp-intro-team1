#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "../include/matrix.h"
#include "../include/block_matrix.h"
#include "../include/get_walltime.h"
#include "../include/mpi_matrix_multiply.h"

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

	MPI_Init(&argc, &argv);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (argc != 5) {
        perror("Wrong number of arguments! Please reenter.\n");
        perror("Arguments:\nN_min N_max interval ouput_file\n");
        return -1;
    }

    const int repeat = 20; // Repeat the calculation to obtain the average flops/s
    const int N_min = atoi(argv[1]); // Second element, refers to the smallest matrix size in computation
    const int N_max = atoi(argv[2]); // Third element of our input string, refers to the largest matrix size, required to be 1 to 10 million
    const int interval = atoi(argv[3]); // atoi converts string to integer

    if (N_max < 1 || interval < 1) {
        perror("Matrix size or interval too small!\n");
        return -1;
    }

    double start_time, end_time, end_time2, end_time3;
    double total_time[N_max];
    double total_time_parallel[N_max];
	double total_time_mpi[N_max];
    double Gflop[N_max]; // Total number of floating point operations
    double performance[N_max]; // Performance time
    double performance_parallel[N_max]; // Performance time for parallel multiply
	double performance_mpi[N_max]; // Performance for MPI multiply
    
    // Output N and performance arrays for plotting
    FILE *odata; // Output data

    odata = fopen(argv[4], "w"); // Third element of our input string, refers to output file name
                                // Open file in write mode
    if (odata == NULL) {
        perror("Error in opening file");
        return -1;
    }
    
    fprintf(odata, "%s,%s,%s\n", "matrix size", "GFLOPS/s", "parallel_GFLOP/s", "mpi_GFLOP/s"); // Print header

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

        for (int i = 0; i < repeat; i++) {
            block_matrix_multiply_parallel(matA, matB, &matC);
        }
        
        get_walltime(&end_time2);

        for (int i = 0; i < repeat; i++) {
            mpi_matrix_multiply(matA, matB, &matC, rank, world_size);
        }
        
        get_walltime(&end_time3);

        //print_matrix(matC);
        
        block_matrix_free(&matA);
        block_matrix_free(&matB);
        block_matrix_free(&matC);
        
        // Performance calculation, GFLOPS/s
        total_time[N-1] = (end_time - start_time) / repeat; // Average time of each repart
        total_time_parallel[N-1] = (end_time2 - end_time) / repeat;
        total_time_mpi[N-1] = (end_time3 - end_time2) / repeat;
        Gflop[N-1] = (matA.rows * matB.cols * matA.cols +
                      matA.rows * matB.cols * (matA.cols - 1)) / 1000000000.0; // Change unit to GFLOPS
        performance[N-1] = Gflop[N-1] / total_time[N-1]; // Compute GFLOPS/s
        performance_parallel[N-1] = Gflop[N-1] / total_time_parallel[N-1]; // Compute GFLOPS/s
        performance_mpi[N-1] = Gflop[N-1] / total_time_mpi[N-1]; // Compute GFLOPS/s
        
        fprintf(odata, "%d,%f,%f,%f\n", N, performance[N-1], performance_parallel[N-1], performance_mpi[N-1]);
    }

    fclose(odata);
   	
	MPI_Finalize();
 
    return 0;
}
