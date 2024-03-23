all: matmul matmul_test

objects=src/main.o src/matrix.o src/block_matrix.o src/get_walltime.o

matmul_test: src/matmul_test.o src/matrix.o src/block_matrix.o
	gcc src/matmul_test.o src/matrix.o src/block_matrix.o -o matmul_test

src/matmul_test.o: src/matmul_test.c src/block_matrix.o include/block_matrix.h
	gcc -c src/matmul_test.c -o src/matmul_test.o

matmul: $(objects)
	gcc $(objects) -o matmul

src/main.o: src/main.c
	gcc -c src/main.c -o src/main.o

src/matrix.o: src/matrix.c include/matrix.h
	gcc -c src/matrix.c -o src/matrix.o

src/block_matrix.o: src/block_matrix.c include/block_matrix.h
	gcc -c src/block_matrix.c -o src/block_matrix.o

src/get_walltime.o: src/get_walltime.c 
	gcc -c src/get_walltime.c -o src/get_walltime.o

