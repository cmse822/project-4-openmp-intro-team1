// matrix_multiply.h
#ifndef MATRIX_MULTIPLY_H
#define MATRIX_MULTIPLY_H

#include "block_matrix.h"

// Function prototypes
void hybrid_mpi_omp_matrix_multiply(block_matrix_t a, block_matrix_t b, block_matrix_t *c, int rank, int world_size);
void block_matrix_multiply_parallel(block_matrix_t a, block_matrix_t b, block_matrix_t *c);

#endif