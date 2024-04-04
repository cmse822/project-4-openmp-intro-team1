#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/block_matrix.h"

void mpi_matrix_multiply(block_matrix_t a, block_matrix_t b, block_matrix_t *c, int rank, int world_size){
	/* Parallelization strategy: 
	 * Split A into groups of rows. Mutliply this by all of B on each node,
	 * and this forms a group of rows of C. Send all rows to node zero to concatenate the matrix together.
	 */
    int rows_per_rank, this_node_row_min, this_node_row_max;
    int ws = (world_size <= a.rows) ? world_size : a.rows;

    if (a.rows % ws == 0) {
        rows_per_rank = a.rows / ws;
        this_node_row_min = rank * rows_per_rank;
        this_node_row_max = this_node_row_min + rows_per_rank - 1;
    } else {
        int x = (int)ceil((double)a.rows / (double)ws);
        int m = a.rows - (ws * (x - 1));
        if (rank < m) {
            rows_per_rank = x;
            this_node_row_min = rank * rows_per_rank;
            this_node_row_max = this_node_row_min + rows_per_rank - 1;
        } else {
            rows_per_rank = x - 1;
            this_node_row_min = m * x + (rank - m) * rows_per_rank;
            this_node_row_max = this_node_row_min + rows_per_rank - 1;
        }
    }

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
    MPI_Barrier(MPI_COMM_WORLD);

    // Make a chunk of C to store the partial multiplication result.
    block_matrix_t chunk_c;
    chunk_c.rows = chunk_a.rows;
    chunk_c.cols = b.cols;
    block_matrix_alloc(&chunk_c);

    // Multiply the chunk of A by all of B using the existing OpenMP function.
    block_matrix_multiply(chunk_a, b, &chunk_c);

    // Gather the chunks from all processes to rank 0.
    int send_counts_per_rank = chunk_c.rows * chunk_c.cols;
    int *recv_elements_count = NULL;
    int *displs = NULL;
    float *gathered_chunk_c = NULL;

    if (rank == 0) {
        recv_elements_count = (int *)malloc(a.rows * sizeof(int));
        displs = (int *)malloc(a.rows * sizeof(int));
    }

    MPI_Gather(&send_counts_per_rank, 1, MPI_INT, recv_elements_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int displs_count = 0;
        for (int i = 0; i < a.rows; ++i) {
            displs[i] = displs_count;
            displs_count += recv_elements_count[i];
        }

        gathered_chunk_c = (float *)malloc(b.cols * a.rows * sizeof(float));
    }

    MPI_Gatherv(chunk_c.data, send_counts_per_rank, MPI_FLOAT, gathered_chunk_c, recv_elements_count, displs, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    // Assign the gathered chunks to the elements of C on rank 0.
    if (rank == 0) {
        int proc = 0;
        int row_in_chunk = 0;
        int index = 0;
        for (int i = 0; i < a.rows; ++i) {
            for (int j = 0; j < b.cols; ++j) {
                proc = i / rows_per_rank;
                row_in_chunk = i % rows_per_rank;
                index = displs[proc] + row_in_chunk * b.cols + j;
                c->data[i * b.cols + j] = gathered_chunk_c[index];
            }
        }
    }

    block_matrix_free(&chunk_a);
    block_matrix_free(&chunk_c);

}
