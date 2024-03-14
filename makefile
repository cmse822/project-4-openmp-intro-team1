all: matmul

matmul: main.o matrix.o block_matrix.o get_walltime.o
	gcc main.o matrix.o block_matrix.o get_walltime.o -o matmul

main.o: main.c
	gcc -c main.c -o main.o

matrix.o: matrix.c matrix.h
	gcc -c matrix.c -o matrix.o

block_matrix.o: block_matrix.c block_matrix.h
	gcc -c block_matrix.c -o block_matrix.o

get_walltime.o: get_walltime.c 
	gcc -c get_walltime.c -o get_walltime.o

