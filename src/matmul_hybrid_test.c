#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include "../include/hybrid_mpi_omp_matrix_multiply.h"
#include "../include/mpi_matrix_multiply.h"
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

    MPI_Init(&argc, &argv);
    int world_size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc != 2) {
        perror("Wrong number of arguments! Please reenter.\n");
        perror("Arguments:\nN_min N_max interval ouput_file");
        return -1;
    }

    const int repeat = 20; // Repeat the calculation to obtain the average flops/s
    // const int N_min = atoi(argv[1]); // Second element, refers to the smallest matrix size in computation
    // const int N_max = atoi(argv[2]); // Third element of our input string, refers to the largest matrix size, required to be 1 to 10 million
    // const int interval = atoi(argv[3]); // atoi converts string to integer
    const char* output_file = argv[1];
    int N_values[] = {10, 50, 100, 200};
    int N_values_size = sizeof(N_values) / sizeof(N_values[0]);

    // if (N_max < 1 || interval < 1) {
    //     perror("Matrix size or interval too small!\n");
    //     return -1;
    // }

    FILE *odata;
    if (rank == 0) {
        odata = fopen(output_file, "w");
        if (odata == NULL) {
            perror("Error in opening file");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }

    int min_threads = 1, max_threads = 128;
    double start_time, end_time;

    if (rank == 0) {
        for (int t = min_threads; t <= max_threads; t *= 2) {
            fprintf(odata, "%d,", t);
        }
        fprintf(odata, "\n");
    }

    for (int i = 0; i < N_values_size; i++) {
        int N = N_values[i];
        
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
        for (int t = min_threads; t <= max_threads; t *= 2) {
            omp_set_num_threads(t);
            double start_time, end_time;
            get_walltime(&start_time);
            for (int i = 0; i < 20; i++) {
                hybrid_mpi_omp_matrix_multiply(matA, matB, &matC, rank, world_size);
            }
            get_walltime(&end_time);
            double total_time = (end_time - start_time) / 20;
            if (rank == 0) {
                fprintf(odata, "%.6f,", total_time);
            }
        }

        if (rank == 0) {
            fprintf(odata, "\n");
        }

        block_matrix_free(&matA);
        block_matrix_free(&matB);
        block_matrix_free(&matC);
    }

    if (rank == 0 && odata != NULL) {
        fclose(odata);
    }

    MPI_Finalize();
    return 0;
}
