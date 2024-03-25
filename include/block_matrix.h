// Get the i, j element of a matrix mat.
// here we use macro instead of function
#define matrix_get(mat, i, j) (mat.data[i * mat.cols + j])
#define matrix_set(mat_ptr, i, j, value) mat_ptr->data[i * mat_ptr->cols + j] = value

/* The matrix is stored as a single block
 * in memory. It is stored such that all of the values
 * for row 1 come first, then all of the values for row2, etc.
 */
typedef struct block_matrix_s {
	size_t rows;
	size_t cols;
	float *data;
} block_matrix_t;

// Allocate memory for a matrix of size 'rows' x 'cols
void block_matrix_alloc(block_matrix_t *mat);

// Free memory for a matrix
void block_matrix_free(block_matrix_t *mat);

// Fill a matrix with constant float value
void fill_constant_block_matrix(block_matrix_t *mat, float value);

// Fill a matrix with random entries between 0 and 1
//void fill_random_block_matrix(block_matrix_t *mat);

// Execute C = C + A * B
void block_matrix_multiply(block_matrix_t a, block_matrix_t b, block_matrix_t *c);
void block_matrix_multiply_parallel(block_matrix_t a, block_matrix_t b, block_matrix_t *c);
void write_matrix_to_csv(const char* filename, block_matrix_t mat);