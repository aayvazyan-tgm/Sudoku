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


	/**************************************************************************
	 * Pull two files in from the standard library. 
	 */

	/* Gets us the I/O stuff: printf, fprintf, fopen, stdout, stderr, etc. */
	#include <stdio.h>

	/* Gets us exit() */
	#include <stdlib.h>


	/**************************************************************************
	 * Define the size of the puzzle so we avoid "magic numbers" in the
	 * code below.   
	 */
	enum {

	  /* The number of rows/cols of a block. */
	  bsize = 3,

	  /* The number of rows/cols of the entire puzzle. */
	  psize = bsize*bsize

	};

	/**************************************************************************
	 * String for converting between integer puzzle cell values (0-9) and
	 * printable characters. Note that empty cell (value 0) maps to '.'
	 */
/* etwas geaendert . auf 0 */
	const char value_chars[psize+1] = "0123456789";

	/**************************************************************************
	 * Define some global variables that affect program output. 
	 */

	/* Set this to 1 for want detailed output: */
	const int verbose = 0;

	/* Set this to 1 if you just want the first puzzle solution, 2 if you
	   just want the first 2, etc. 0 means find all solutions. */
	const int max_solutions = 0;

	/**************************************************************************
	 * Here we set up two type definitions for our program. A "typedef" is
	 * just creating another name for a type, so 
	 *
	 *  typedef int foo_t;
	 *
	 * means the compiler will treat "foo_t" and "int" equivalently from
	 * now on.
	 */

	/* Type to store a value in a puzzle cell. Needs to have a maximum
	   value greater than or equal to psize. */
	typedef unsigned char value_t;

	/* Type to store a combination, or set of values, represented as a bit
	   vector. Needs to have at least psize bits. Bit i is set to one
	   if the set contains value (i-1). */
	typedef unsigned short value_set_t;

	/**************************************************************************
	 * Convert a character to the corresponding value in range
	 * (0..psize) by searching in the value_chars string.
	 */
	value_t char_to_value(char c) {
	  value_t value;
	  for (value=1; value<=psize; ++value) {
		if (value_chars[value] == c) {
		  return value;
		}
	  }
	  return 0;
	}

	/**************************************************************************
	 * Fill a cell in the puzzle with a value, and double-check that we're
	 * not overwriting a non-zero value with a non-zero value (which would be
	 * an error).
	 */
	void puzzle_fill_cell(value_t puzzle[psize][psize],
						  int row, int col, value_t value) {


	  /* See if both are non-zero */
	  if (value && puzzle[row][col]) {

		/* Warn the user, and exit. */
		fprintf(stderr, "cell (%d, %d) already has a value!", row, col);
		exit(-1);

	  }

	  /* Now just fill the cell. */
	  puzzle[row][col] = value;

	}

	/**************************************************************************
	 * Print the puzzle to the given stream.  To print to the console,
	 * call puzzle_write(puzzle, stdout).  
	 */
/* Past so */
	void puzzle_write(value_t puzzle[psize][psize], FILE* stream) {

	  int row, col;
	  
	  for (row=0; row<psize; ++row) {
		for (col=0; col<psize; ++col) {
		  value_t value = puzzle[row][col];
		  fputc(value_chars[value], stream);
		}
		fputc('\n', stream);
	  }

	}

	/**************************************************************************
	 * Read the puzzle from the stream. To read from console input (typing
	 * each line), call puzzle_read(puzzle, stdin).  To read from a file,
	 * see puzzle_read_file below.
	 */
	void puzzle_read(value_t puzzle[psize][psize], FILE* stream) {

	  int row, col;
	  char c;

	  /* For each puzzle row: */
	  for (row=0; row<psize; ++row) {

		/* For each puzzle column: */
		for (col=0; col<psize; ++col) {
		  /* Get the next character. */
		  c = fgetc(stream);

		  /* See if we are at end-of-file. If so, warn and exit. */
		  if (feof(stream)) {
			fprintf(stderr, "end of file before reading entire puzzle\n");
			exit(-1);
		  }

		  /* See if we have a newline before we have read all characters
			 in this row. If so, warn and exit. */
		  if (c == '\n') {
			fprintf(stderr, "line ended before reading all characters in row\n");
			exit(-1);
		  }
		  
/* if/else Hinzugefuegt */
		  if(c == ',') {
			col--;
		  } else {

			  /* Convert the character to the corresponding value, and store
				 it in the puzzle. */
			  puzzle[row][col] = char_to_value(c);
		  }

		} 

		/* Done reading cols in this row. Keep getting (and throwing away)
		   characters until we see a newline. If we see an end-of-file
		   condition, we'll deal with it back up at the top of the loop if
		   we get back there.*/
		while (c != '\n' && !feof(stream)) {
		  c = fgetc(stream);
		}

	  }


	}

	/**************************************************************************
	 * Read a puzzle from a file. This just opens up a stream, reads the
	 * stream using the above function, and closes the file back up again.
	 */
	void puzzle_read_file(value_t puzzle[psize][psize], const char* filename) {

	  /* Open our file for reading. */
	  FILE* stream = fopen(filename, "r");

	  /* Check if it didn't open. If so, warn and exit. */
	  if (!stream) { 
		fprintf(stderr, "couldn't open file %s\n", filename);
		exit(-1);
	  }

	  /* Read from the stream */
	  puzzle_read(puzzle, stream);

	  /* Be nice and close the file when we're done. */
	  fclose(stream);
	  
	}

	/**************************************************************************
	 * Search through the puzzle for the first empty cell. Note that
	 * row_out and col_out arguments are passed by reference.
	 */
	int puzzle_find_empty(value_t puzzle[psize][psize], 
						  int* row_out, int* col_out) {

	  int row, col;

	  for (row=0; row<psize; ++row) {
		for (col=0; col<psize; ++col) {

		  value_t value = puzzle[row][col];
		  if (!value) {
			/* Found an empty cell, record its position and return
			   success. */
			*row_out = row;
			*col_out = col;
			return 1;
		  }
		}
	  }

	  /* No empty cell found -- Deliberately assign bad values to row_out
		 and col_out if no cells were empty -- they ought to be ignored
		 anyways. */
	  *row_out = psize;
	  *col_out = psize;
	  return 0;
	  
	}

	/**************************************************************************
	 * Check if this value has already been seen in the row, column, or
	 * block that we're inspecting.  If the value is zero, the cell is
	 * empty and we can skip over it.  If the value is non-zero, we see if
	 * the value has already been observed.  If so, we return 0; otherwise
	 * we mark the value as seen and return 0.
	 */
	int region_check_and_mark(value_set_t* region, value_t value) {
	  if (value) {
		int marker = (1 << (value-1));
		if (*region & marker) {
		  return 0;
		} else {
		  *region = *region | marker;
		}
	  }
	  return 1;
	}

	/**************************************************************************
	 * Returns 1 if the puzzle is a valid (partial) solution. This means that
	 * every value from 1 to psize:
	 *
	 *   - appears in every row no more than once
	 *   - appears in every column no more than once
	 *   - appears in every bsize-by-bsize block no more than once
	 *
	 * Note that a valid puzzle with no empty spaces is solved.  
	 */
	int puzzle_is_valid(value_t puzzle[psize][psize]) {

	  int row, col, start_row, start_col, block_row, block_col;
	  value_set_t region;

	  /* Validate each row. */
	  for (row=0; row<psize; ++row) {

		/* Start by clearing our region bits: */
		region = 0;

		/* Check each column in the row. */
		for (col=0; col<psize; ++col) {
		  if (!region_check_and_mark(&region, puzzle[row][col])) {
			return 0;
		  }
		}

	  }

	  /* Rows were valid, now validate each column. */
	  for (col=0; col<psize; ++col) {

		/* Start by clearing out our region bits. */
		region = 0;

		/* Check each row in the column. */
		for (row=0; row<psize; ++row) {
		  if (!region_check_and_mark(&region, puzzle[row][col])) {
			return 0;
		  }
		}
	  }

	  /* Rows and columns were valid, now check each block. */

	  /* Outer two loops iterate over each block. Blocks start every bsize
		 rows/cols. */
	  for (start_row=0; start_row<psize; start_row+=bsize) {
		for (start_col=0; start_col<psize; start_col+=bsize) {

		  /* Clear out our region bits for this block. */
		  region = 0;

		  /* Inner two loops iterate within each block. */
		  for (block_row=0; block_row<bsize; ++block_row) {
			for (block_col=0; block_col<bsize; ++block_col) {

			  row = start_row + block_row;
			  col = start_col + block_col;

			  if (!region_check_and_mark(&region, puzzle[row][col])) {
				return 0;
			  }

			}
		  } /* within each block */

		}
	  } /* for each block */

	  /* Rows, columns, and blocks were valid -- so puzzle is valid! */
	  return 1;

	}

	/**************************************************************************
	 * Finally, we can solve our puzzle.  This function calls itself
	 * recursively (compile with verbose=1 to get detailed output about
	 * what it's doing).  This is an example of classical backtracking
	 * search: try each possible thing -- if you get to a dead-end,
	 * backtrack to the previous decision and try another option.
	 * Eventually you'll solve the puzzle or run out of options.
	 *
	 * Function arguments:
	 * 
	 *   - puzzle is our puzzle, which we modify along the way.
	 *     Because it's passed by value, we need to undo every change
	 *     after we try it, so that we don't pollute the puzzle as 
	 *     we try different possibilities. 
	 *
	 *   - depth is the search depth (how many times the function has
	 *     called itself).
	 *
	 *   - moves (passed by reference) keeps track of how many moves
	 *     we've made in solving the puzzle (including those subsequently
	 *     undone).
	 *
	 *   - solutions (passed by reference) keeps track of how many
	 *     solutions we've seen so far.
	 */
	void puzzle_solve(value_t puzzle[psize][psize], 
					  int depth, int* moves, int* solutions) {

	  int have_empty, empty_row, empty_col;
	  value_t value;

	  /* Note we should never go deeper than the number of cells in the
		 puzzle.  If we did, it's a sign that something went horribly
		 wrong... */
	  if (depth > psize*psize) {
		fprintf(stderr, "something went wrong, search too deep\n");
		exit(-1);
	  }

	  /* Output the progress so far. */
	  if (verbose) {
		printf("with search depth %d, puzzle is:\n\n", depth);
		puzzle_write(puzzle, stdout);
		printf("\n");
	  }

	  /* First base case: if we're invalid, there can be no further solution
	   * from this point, so return to the caller (backtrack). */
	  if (!puzzle_is_valid(puzzle)) {
		if (verbose) {
		  printf("unsolvable from here, backtracking...\n");
		}
		return;
	  }

	  /* Puzzle was valid, find the first empty cell. */
	  have_empty = puzzle_find_empty(puzzle, &empty_row, &empty_col);

	  /* Second base case: There were no empty cells and puzzle was valid,
	   * so this is a solved puzzle. No point in recursing further! */
	  if (!have_empty) {
		++(*solutions);
		printf("found solution %d in %d moves:\n\n", *solutions, *moves);
		puzzle_write(puzzle, stdout);
		printf("\n");
		return;
	  }

	  /* Puzzle was valid, but not solved yet. So try each value at
	   * the empty cell we identified. */
	  if (verbose) {
		printf("found empty cell at (%d, %d)\n", empty_row, empty_col);
	  }

	  /* For each non-zero value: */
	  for (value=1; value<=psize; ++value) {

		if (verbose) {
		  printf("trying value %c in cell (%d, %d)...\n", 
				 value_chars[value], empty_row, empty_col);
		}

		/* Place the value in the cell and increment our number of moves. */
		puzzle_fill_cell(puzzle, empty_row, empty_col, value);
		++(*moves);

		/* This is the inductive case of the recursion. We have modified
		   the puzzle by adding a new value in one of the cells, and now
		   we need to solve the modified puzzle. */
		puzzle_solve(puzzle, depth+1, moves, solutions);

		/* If the puzzle is solved and we have as many solutions as we
		   need, quit now. */
		if (max_solutions && *solutions >= max_solutions) {
		  return;
		}

		/* Empty the cell again so that backtracking works properly. We do
		 * this inside of the for loop because the puzzle_fill_cell above
		 * would generate an error otherwise. */
		puzzle_fill_cell(puzzle, empty_row, empty_col, 0);

	  }
	  
	}

	/**************************************************************************
	 * Our main function.  If a command line argument is present, try to
	 * open the file specified on the command line; otherwise, read from 
	 * standard input.
	 */
	int main(int argc, char** argv) {

	  value_t puzzle[psize][psize];
	  int moves=0, solutions=0;

	  /* If we have an argument, interpret it as a filename and try to
		 read puzzle from that file. Otherwise, read from the console.  */
	  if (argc > 1) {
		puzzle_read_file(puzzle, argv[1]);
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
	  puzzle_solve(puzzle, 0, &moves, &solutions);

	  /* Summarize the # of moves and solutions. */
	  printf("total %d moves, %d solutions\n", moves, solutions);

	  /* Return 0 to indicate no errors occurred. */
	  return 0;

	}