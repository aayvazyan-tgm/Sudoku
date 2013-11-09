/*
  Name			: sudokuheader.h
  Author		: Vennesa Belinic
  Version		: 2013-11-08
  Description	: Die Defines, Includes und Prototypen die im sudoku.c verwendet werden
 */

/**************************************************************************
	 * Pull two files in from the standard library.
	 */
#ifndef __SUDOKUHEADER__
#define __SUDOKUHEADER__

#include <stdio.h>

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


/* Prototypen hinzufuegt */
	value_t char_to_value(char c);
	void puzzle_fill_cell(value_t puzzle[psize][psize], int, int, value_t);
	void puzzle_fill_cell(value_t puzzle[psize][psize], int, int, value_t);
	void puzzle_write(value_t puzzle[psize][psize], FILE*);
	void puzzle_read(value_t puzzle[psize][psize], FILE*);
	void puzzle_read_file(value_t puzzle[psize][psize], const char*);
	int puzzle_find_empty(value_t puzzle[psize][psize], int*, int*);
	int region_check_and_mark(value_set_t*, value_t);
	int puzzle_is_valid(value_t puzzle[psize][psize]);
	void puzzle_write_csv(value_t puzzle[psize][psize], FILE*);
	void puzzle_solve(value_t puzzle[psize][psize], int, int*, int*, FILE*);


#endif
