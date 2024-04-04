#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
//#include "../include/block_matrix.h"
#include "../include/mpi_matrix_multiply.h"

void mpi_matrix_multiply(block_matrix_t a, block_matrix_t b, block_matrix_t *c, int rank, int world_size){
	/* Parallelization strategy: 
	 * Split A into groups of rows. Mutliply this by all of B on each node,
	 * and this forms a group of rows of C. Send all rows to node zero to concatenate the matrix together.
	 */
	int rows_per_rank, this_node_row_min, this_node_row_max;
	//rows_per_rank = (int)ceil((double)a.rows / (double)world_size);
	rows_per_rank = a.rows / world_size;
	//if (rank == 0) printf("%d rows per rank\n", rows_per_rank);
	this_node_row_min = rank * rows_per_rank;
	if (this_node_row_min >= a.rows - 1) this_node_row_min = a.rows - 1;
	this_node_row_max = (rank + 1) * rows_per_rank - 1;
	if (this_node_row_max >= a.rows - 1) this_node_row_max = a.rows - 1;
	if (rank == world_size - 1) this_node_row_max = a.rows - 1;
	//printf("rank %d rows %d %d\n", rank, this_node_row_min, this_node_row_max);
	
	// Store a chunk of A in a separate matrix.
	block_matrix_t chunk_a;
	chunk_a.rows = this_node_row_max - this_node_row_min + 1;
	chunk_a.cols = a.cols;
	block_matrix_alloc(&chunk_a);
	int i_local = 0;
	for (int i = this_node_row_min; i <= this_node_row_max; i++) {
		for (int j = 0; j < chunk_a.cols; j++) {
			matrix_set((&chunk_a), i_local, j, matrix_get(a, i, j));
		}
		i_local++;
	}
	//printf("rank %d Finished allocation\n", rank);

	// Make a chunk of C to store the partial multiplication result.
	block_matrix_t chunk_c;
	chunk_c.rows = chunk_a.rows;
	chunk_c.cols = b.cols;
	block_matrix_alloc(&chunk_c);
	// Multiply the chunk of A by all of B.
	block_matrix_multiply(chunk_a, b, &chunk_c);
	//printf("rank %d Finished multiply\n", rank);

	// Send all chunks to rank 0.
	int *recv_elements_count = NULL;
	int *displs = NULL;


	if (rank == 0) {
		//printf("Allocating arrays\n");
		//printf("comm size is %d\n", world_size);
    	recv_elements_count = (int *)malloc(world_size * sizeof(int));
		displs = (int *)malloc(world_size * sizeof(int));
	}
	
	int chunk_c_elements = chunk_c.rows * chunk_c.cols;
	//printf("rank %d has %d elements\n", rank, chunk_c_elements);
	MPI_Barrier(MPI_COMM_WORLD);
	int status = MPI_Gather(&chunk_c_elements, 1, MPI_INT, recv_elements_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (status != MPI_SUCCESS) {
		fprintf(stderr, "mpi gather error code %d\n", status);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	//printf("rank %d Finsished gather.\n", rank);

	// check number of elements
	int total_elements = 0;
	if (rank == 0) {
		for (int r = 0; r < world_size; r++) 
			total_elements += recv_elements_count[r];
		//printf("total elements = %d\n", total_elements);
		if (total_elements != b.cols * a.rows) {
			fprintf(stderr, "Non-matching dimensions of total elements %d from rank and %d from matrices", total_elements, b.cols * a.rows);
			exit(-1);
		}
	}

	if (rank == 0) {
		displs = (int *)malloc(world_size * sizeof(int));
		displs[0] = 0;
		for (int i = 0; i < world_size; ++i) {
			displs[i] = displs[i-1] + recv_elements_count[i-1];
		}
		for (int i = 0; i < world_size; ++i) {
			if (displs[i] > total_elements) {
				fprintf(stderr, "displacement %d from rank %d exceeds matrix size %d\n", displs[i], i, total_elements);
				exit(-1);
			}
		}
	}

	float *gathered_chunk_c = NULL;
	if (rank == 0) {
		gathered_chunk_c = (float *)malloc(b.cols * a.rows * sizeof(float));
	}

	MPI_Barrier(MPI_COMM_WORLD);
	status = MPI_Gatherv(chunk_c.data, chunk_c_elements, MPI_FLOAT, 
				gathered_chunk_c, recv_elements_count, displs, MPI_FLOAT, 0, MPI_COMM_WORLD);
	if (status != MPI_SUCCESS) {
		fprintf(stderr, "Error in gatherv\n");
		exit(-1);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	//printf("rank %d Finished gatherv\n", rank);
	
	// Assign these chunks to the elements of C.
	if (rank == 0) {
		for (int i = 0; i < a.rows; ++i) {
			for (int j = 0; j < b.cols; ++j) {
				//int proc = i / rows_per_rank;
				//int row_in_chunk = i % rows_per_rank;
				//int index = displs[proc] + row_in_chunk * b.cols + j;
				int index = i + b.cols * j;
				if (index > total_elements - 1) {
					fprintf(stderr, "Index %d out of range for array size %d\n", index, total_elements);
					exit(-1);
				}
				matrix_set(c, i, j, gathered_chunk_c[index]);
			}
		}
	}

	block_matrix_free(&chunk_a);
	block_matrix_free(&chunk_c);

	if (rank == 0) {
        free(recv_elements_count);
        free(displs);
        free(gathered_chunk_c);
    }
}
