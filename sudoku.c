/*
  Name			: sudoku.c
  Author		: Vennesa Belinic
  Version		: 2013-11-05
  Description	: Findet sich im darunterliegendem Kommentar.
  	  	  	  	  File von Matt Zucker editiert.
 */

	/**************************************************************************
	 * sudoku.c                   Matt Zucker                        11/3/2010
	 * 
	 * Implements a Sudoku solver in C.  The method used here, brute-force
	 * backtracking search, is surprisingly effective on modern PC's.
	 * From Wikipedia:
	 *
	 *   "Sudoku is a logic-based, combinatorial number-placement
	 *    puzzle. The objective is to fill a 9x9 grid with digits so that
	 *    each column, each row, and each of the nine 3x3 sub-grids that
	 *    compose the grid (also called "boxes", "blocks", "regions", or
	 *    "sub-squares") contains all of the digits from 1 to 9. The
	 *    puzzle setter provides a partially completed grid, which
	 *    typically has a unique solution."
	 *
	 * Aside from illustrating how inane Sudoku actually is, this C file
	 * demonstrates a number of C programming concepts and skills that I'd
	 * like you to become familiar with if you're not already:
	 *
	 *   - program control flow (if, else, for, while, etc.)
	 *   - error checking to encode our assumptions and to validate input
	 *   - program return values (exit, return from main)
	 *   - integer types and sizes
	 *   - arrays, including multidimensional arrays
	 *   - iterating over all elements of an array
	 *   - null-terminated strings and character arrays
	 *   - searching for specific characters within a string
	 *   - handling command-line arguments
	 *   - bitwise operations
	 *   - file I/O (input and output), including fprintf and printf
	 *   - passing function arguments by reference vs. passing by value
	 *   - recursion
	 *   - constant values using variables and enums
	 *   - typedefs to define variable types
	 *   - using const for immutable strings
	 *
	 * We will be going over all of these, to some extent, in class. 
	 *
	 * Sudoku puzzles are represented as 9 lines of text, with '1'-'9' for
	 * filled cells, and '.' for empty cells.  Any characters past the 9th
	 * one on each line are ignored.
	 *
	 */
#include "sudokuheader.h"
#include <string.h>
#include <stdio.h>


	/**************************************************************************
	 * Our main function.  If a command line argument is present, try to
	 * open the file specified on the command line; otherwise, read from 
	 * standard input.
	 */

	int main(int argc, char** argv) {
		xsudoku=0;
		if(argc == 1 || argc > 4)
			printf("Arguments: [normal|x-sudoku] inputfile.csv outputfile.csv\n");
		else if(argc == 2 || argc == 4) {
			if(strcmp(argv[1],"x-sudoku") == 0){
				xsudoku=1;
			}
			if(strcmp(argv[1],"normal") == 0||strcmp(argv[1],"x-sudoku") == 0) {
				value_t puzzle[psize][psize];
				int moves=0, solutions=0;

				/* If we have an argument, interpret it as a filename and try to
				 read puzzle from that file. Otherwise, read from the console.  */
				if (argc >= 3) {
				puzzle_read_file(puzzle, argv[2]);
				} else {
				printf("reading from standard input:\n");
				puzzle_read(puzzle, stdin);
				}

				/* Display the initial puzzle as a nice sanity check. */
				printf("got initial puzzle:\n\n");
				puzzle_write(puzzle, stdout);
				printf("\n");

				/* Display a sad message if the initial puzzle was invalid. */
				if (!puzzle_is_valid(puzzle)) {
				printf("initial puzzle invalid!\n");
				/* Return -1 from main is basically the same as exit(-1) */
				return -1;
				}

				/* Check for solutions. */

	//Hinzugefuegt
				if(argc == 4) {
					FILE* stream = fopen(argv[3], "w");
					puzzle_solve(puzzle, 0, &moves, &solutions, stream);
				}

				/* Summarize the # of moves and solutions. */
				printf("total %d moves, %d solutions\n", moves, solutions);
			}


		}

	/* Return 0 to indicate no errors occurred. */
	return 0;

	}
