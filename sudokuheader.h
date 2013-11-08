/*
  Name			: sudokuheader.h
  Author		: Vennesa Belinic
  Version		: 2013-11-08
  Description	: Die Defines, Includes und Prototypen die im sudoku.c verwendet werden
 */

/**************************************************************************
	 * Pull two files in from the standard library.
	 */

	/* Gets us the I/O stuff: printf, fprintf, fopen, stdout, stderr, etc. */
	#include <stdio.h>

	/* Gets us exit() */
	#include <stdlib.h>

/* Hinzugefuegt von mir, da ich Stringfunktionen benoetige */
	#include <string.h>


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
