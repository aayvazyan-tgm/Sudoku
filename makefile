all: sudoku 

sudoku: sudoku.c
	gcc -O3 -Wall -o $@ $^
