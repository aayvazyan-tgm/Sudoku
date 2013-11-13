/*
 Name			: sudokuheader.h
 Author		    : Vennesa Belinic, Ari Ayvazyan
 Version		: 2013-11-08
 Description	: Die Funktionen die in sudoku.c verwendet werden
 */

#include "sudokuheader.h"

/* Gets us the I/O stuff: printf, fprintf, fopen, stdout, stderr, etc. */
#include <stdio.h>

/* Gets us exit() */
#include <stdlib.h>

/* Hinzugefuegt von mir, da ich Stringfunktionen benoetige */
#include <string.h>

/**************************************************************************
 * String for converting between integer puzzle cell values (0-9) and
 * printable characters. Note that empty cell (value 0) maps to '.'
 */
/* etwas geaendert . auf 0 */
const char value_chars[psize + 1] = "0123456789";

/**************************************************************************
 * Define some global variables that affect program output.
 */

/* Set this to 1 for want detailed output: */
const int verbose = 0;

/* Set this to 1 if you just want the first puzzle solution, 2 if you
 just want the first 2, etc. 0 means find all solutions. */
const int max_solutions = 0;

/**************************************************************************
 * Convert a character to the corresponding value in range
 * (0..psize) by searching in the value_chars string.
 */
value_t char_to_value(char c) {
	value_t value;
	for (value = 1; value <= psize; ++value) {
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
void puzzle_fill_cell(value_t puzzle[psize][psize], int row, int col,
		value_t value) {

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
void puzzle_write(value_t puzzle[psize][psize], FILE* stream) {

	int row, col;

	for (row = 0; row < psize; ++row) {
		for (col = 0; col < psize; ++col) {
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
	for (row = 0; row < psize; ++row) {

		/* For each puzzle column: */
		for (col = 0; col < psize; ++col) {
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
				fprintf(stderr,
						"line ended before reading all characters in row\n");
				exit(-1);
			}

	/* Hinzugefuegt */
			if (c == ',') {
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
int puzzle_find_empty(value_t puzzle[psize][psize], int* row_out, int* col_out) {

	int row, col;

	for (row = 0; row < psize; ++row) {
		for (col = 0; col < psize; ++col) {

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
		int marker = (1 << (value - 1));
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
	for (row = 0; row < psize; ++row) {

		/* Start by clearing our region bits: */
		region = 0;

		/* Check each column in the row. */
		for (col = 0; col < psize; ++col) {
			if (!region_check_and_mark(&region, puzzle[row][col])) {
				return 0;
			}
		}

	}

	/* Rows were valid, now validate each column. */
	for (col = 0; col < psize; ++col) {

		/* Start by clearing out our region bits. */
		region = 0;

		/* Check each row in the column. */
		for (row = 0; row < psize; ++row) {
			if (!region_check_and_mark(&region, puzzle[row][col])) {
				return 0;
			}
		}
	}

	/* Rows and columns were valid, now check each block. */

	/* Outer two loops iterate over each block. Blocks start every bsize
	 rows/cols. */
	for (start_row = 0; start_row < psize; start_row += bsize) {
		for (start_col = 0; start_col < psize; start_col += bsize) {

			/* Clear out our region bits for this block. */
			region = 0;

			/* Inner two loops iterate within each block. */
			for (block_row = 0; block_row < bsize; ++block_row) {
				for (block_col = 0; block_col < bsize; ++block_col) {

					row = start_row + block_row;
					col = start_col + block_col;

					if (!region_check_and_mark(&region, puzzle[row][col])) {
						return 0;
					}

				}
			} /* within each block */

		}
	} /* for each block */

	/* If xsudoku is selected, check the diagonal values too */
	if(xsudoku==1){
		int horizontal;
		int vertikal;
		value_t zero=0;
		//Checks the diagonal values starting from the top left
		for(vertikal=0;vertikal<psize;vertikal++){
			for(horizontal=0;horizontal<psize;horizontal++){
				if(vertikal==horizontal)continue;
				if(puzzle[vertikal][vertikal]==zero)continue;
				if(puzzle[horizontal][horizontal]==zero)continue;
				if(puzzle[vertikal][vertikal]==puzzle[horizontal][horizontal])
					{
					//printf("%d,%d",puzzle[vertikal][vertikal],puzzle[horizontal][horizontal]);
					return 0;
					}
			}
		}
		for(vertikal=psize;vertikal<psize;vertikal--){
			for(horizontal=0;horizontal<psize;horizontal++){
				if(vertikal==horizontal)continue;
				if(puzzle[vertikal][vertikal]==zero)continue;
				if(puzzle[horizontal-psize+1][horizontal]==zero)continue;
				if(puzzle[vertikal][vertikal]==puzzle[horizontal-psize+1][horizontal])return 0;
			}
		}
	}
	/* Rows, columns, and blocks were valid -- so puzzle is valid! */
	return 1;

}

/* Hinzugefuegt */
/*
 * Schreibt die Loesung in ein csv-File
 */
void puzzle_write_csv(value_t puzzle[psize][psize], FILE* stream) {

	int row, col;

	for (row = 0; row < psize; ++row) {
		for (col = 0; col < psize; ++col) {
			value_t value = puzzle[row][col];
			fputc(value_chars[value], stream);
			if (col < (psize - 1))
				fputc(',', stream);
		}
		fputc('\n', stream);
	}

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
/* Hinzufuegen des Parameter stream FILE* */
void puzzle_solve(value_t puzzle[psize][psize], int depth, int* moves,
		int* solutions, FILE* stream) {

	int have_empty, empty_row, empty_col;
	value_t value;

	/* Note we should never go deeper than the number of cells in the
	 puzzle.  If we did, it's a sign that something went horribly
	 wrong... */
	if (depth > psize * psize) {
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

		/* Hinzugefuegt um Loesung in ein csv-File schreibe */
		puzzle_write_csv(puzzle, stream);

		printf("\n");
		return;
	}

	/* Puzzle was valid, but not solved yet. So try each value at
	 * the empty cell we identified. */
	if (verbose) {
		printf("found empty cell at (%d, %d)\n", empty_row, empty_col);
	}

	/* For each non-zero value: */
	for (value = 1; value <= psize; ++value) {

		if (verbose) {
			printf("trying value %c in cell (%d, %d)...\n", value_chars[value],
					empty_row, empty_col);
		}

		/* Place the value in the cell and increment our number of moves. */
		puzzle_fill_cell(puzzle, empty_row, empty_col, value);
		++(*moves);

		/* This is the inductive case of the recursion. We have modified
		 the puzzle by adding a new value in one of the cells, and now
		 we need to solve the modified puzzle. */
		puzzle_solve(puzzle, depth + 1, moves, solutions, stream);

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
