# Define the standard C compiler and MPI compiler
CC=mpicc
MPICC=mpicc
# Compiler flags, enable OpenMP
CFLAGS=-fopenmp
# Linker flags, include math library and enable OpenMP
LDFLAGS=-lm -fopenmp

### Default Target: build matmul and matmul_test programs
all: matmul matmul_test mpi_matmul_test

# List of object files needed to build the targets
objects=src/main.o src/matrix.o src/block_matrix.o src/get_walltime.o src/mpi_matrix_multiply.o

### Target to build the matmul_test executable
matmul_test: src/matmul_test.o src/matrix.o src/block_matrix.o
	$(CC) $(LDFLAGS) src/matmul_test.o src/matrix.o src/block_matrix.o -o matmul_test

src/matmul_test.o: src/matmul_test.c include/block_matrix.h
	$(CC) $(CFLAGS) -c src/matmul_test.c -o src/matmul_test.o

mpi_matmul_test: src/mpi_matmul_test.o src/matrix.o src/block_matrix.o
	$(CC) $(LDFLAGS) src/mpi_matrix_multiply.o src/mpi_matmul_test.o src/matrix.o src/block_matrix.o -o mpi_matmul_test

src/mpi_matmul_test.o: src/mpi_matmul_test.c include/block_matrix.h
	$(CC) $(CFLAGS) -c src/mpi_matmul_test.c -o src/mpi_matmul_test.o

### Target to build the matmul executable, which likely requires MPI for parallel distributed computing
matmul: $(objects)
	$(MPICC) $(objects) -o matmul $(LDFLAGS)

src/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o src/main.o

src/matrix.o: src/matrix.c include/matrix.h
	$(CC) $(CFLAGS) -c src/matrix.c -o src/matrix.o

src/block_matrix.o: src/block_matrix.c include/block_matrix.h
	$(CC) $(CFLAGS) -c src/block_matrix.c -o src/block_matrix.o

src/get_walltime.o: src/get_walltime.c
	$(CC) $(CFLAGS) -c src/get_walltime.c -o src/get_walltime.o

src/mpi_matrix_multiply.o: src/mpi_matrix_multiply.c include/mpi_matrix_multiply.h  # using MPICC for MPI support
	$(MPICC) -c src/mpi_matrix_multiply.c -o src/mpi_matrix_multiply.o $(LDFLAGS)


# Clean up command: remove all object files and executables
clean:
	rm -f src/*.o matmul matmul_test
