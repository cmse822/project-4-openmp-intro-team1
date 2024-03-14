// Header for matrix data type and functions.

/* Matrix is stored as a pointer to a pointer to a float.
 * We will store the array such that A[i][j] is the element
 * at row i and column j.
 */
#include <stdbool.h>

struct matrix_s {
	size_t rows;
	size_t cols;
	float **data;
};

typedef struct matrix_s matrix_t;

void matrix_alloc(matrix_t *mat);

void matrix_free(matrix_t *mat);

// Fill a matrix with constant value.
// i.e. A[i][j] = c for all i, j.
void fill_constant_matrix(matrix_t *mat, float value);

// Fill a matrix with random entries between 0 and 1.
//void fill_random_matrix(matrix *mat);

// Executes C = C + A * B.
bool matrix_multiply(matrix_t a, matrix_t b, matrix_t *c);
