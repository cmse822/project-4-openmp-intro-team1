#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "../include/block_matrix.h" 
#include "../include/mpi_matrix_multiply.h" 

void print_matrix(block_matrix_t mat) {
	for (int i = 0; i < mat.rows; i++) {
		for(int j = 0; j < mat.cols; j++) {
			printf("%f ", matrix_get(mat, i, j));
		}
		printf("\n");
	}
}

int main(int argc, char *argv[]) {
	MPI_Init(&argc, &argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	block_matrix_t a, b, c;
	int size = 4;
	a.rows = size;
	a.cols = size;
	b.rows = size;
	b.cols = size;
	c.rows = a.rows;
	c.cols = b.cols;
	block_matrix_alloc(&a);
	block_matrix_alloc(&b);
	block_matrix_alloc(&c);

	// Set A to the identity, and B randomly.
	// Set C to all zeros.
	for (int i = 0; i < a.rows; i++) {
		for (int j = 0; j < a.cols; j++) {
			if (i == j) 
				matrix_set((&a), i, j, 1.0);
			else
				matrix_set((&a), i, j, 0.0);
		}
	}
	float r;
	for (int i = 0; i < b.rows; i++) {
		for (int j = 0; j < b.cols; j++) {
			r = (float)rand() / (float)RAND_MAX;
			matrix_set((&b), i, j, r);
		}
	}
	fill_constant_block_matrix(&c, 0.0);
	
	if (rank == 0) {
		printf("b = \n");
		print_matrix(b);
	}

	mpi_matrix_multiply(a, b, &c, rank, world_size);

	if (rank == 0) {
		printf("c = \n");
		print_matrix(c);
	}

	block_matrix_free(&a);
	block_matrix_free(&b);
	block_matrix_free(&c);

	MPI_Finalize();
}
