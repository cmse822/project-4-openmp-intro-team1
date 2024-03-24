#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include "../include/block_matrix.h"

void mpi_matrix_multiply(block_matrix_t a, block_matrix_t b, block_matrix_t *c, int rank, int world_size){
	/* Parallelization strategy: 
	 * Split A into groups of rows. Mutliply this by all of B on each node,
	 * and this forms a group of rows of C. Send all rows to node zero to concatenate the matrix together.
	 */
	int rows_per_rank, this_node_row_min, this_node_row_max;
	rows_per_rank = (int)ceil((double)a.rows / (double)world_size);
	this_node_row_min = rank * rows_per_rank;
	this_node_row_max = (rank + 1) * rows_per_rank - 1;
	if (this_node_row_max > a.rows) this_node_row_max = a.rows;
	
	// Store a chunk of A in a separate matrix.
	block_matrix_t chunk_a;
	chunk_a.rows = this_node_row_max - this_node_row_min + 1;
	chunk_a.cols = a.cols;
	block_matrix_alloc(&chunk_a);
	for (int i = this_node_row_min; i <= this_node_row_max; i++) {
		for (int j = 0; j < chunk_a.cols; j++) {
			matrix_set((&chunk_a), i, j, matrix_get(a, i, j));
		}
	}

	// Make a chunk of C to store the partial multiplication result.
	block_matrix_t chunk_c;
	chunk_c.rows = chunk_a.rows;
	chunk_c.cols = b.cols;
	block_matrix_alloc(&chunk_c);
	// Multiply the chunk of A by all of B.
	block_matrix_multiply(chunk_a, b, &chunk_c);

	// Send all chunks to rank 0.
	// Assign these chunks to the elements of C.

	block_matrix_free(&chunk_a);
	block_matrix_free(&chunk_c);
}
