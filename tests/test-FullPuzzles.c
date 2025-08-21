#include "test-FullPuzzles.h"
#include <stdio.h>
#include "../include/gameBoard.h"

#define TEST1 "testPuzzles/test-10x10.txt"
#define TEST1_SOLUTION "testPuzzles/test-10x10-solution.txt"
#define TEST2 "testPuzzles/test-20x20.txt"
#define TEST2_SOLUTION "testPuzzles/test-20x20-solution.txt"
#define TEST3 "testPuzzles/test-30x30.txt"
#define TEST3_SOLUTION "testPuzzles/test-30x30-solution.txt"
#define TEST4 "testPuzzles/test-50x50.txt"
#define TEST4_SOLUTION "testPuzzles/test-50x50-solution.txt"

int testFullPuzzles ()
{
	int failures = 0;	

	return failures;
}

int * generateSolutionGameBoard (FILE * filePtr, int width, int length)
{
	int character, index = 0;
	int * solutionGameBoard = createGameBoard (width, length);

	while ((character = fgetc(filePtr)) != EOF)
	{
		if (character != '#' && character != '.')
			continue;

		else if (character == '#')
			solutionGameBoard[index++] = 1;

		else
			solutionGameBoard[index++] = 0;
	}

	/* Sanity check */

	if (!isSolved(solutionGameBoard, width, length))
		printf("Generating solution gameBoard failure.\n");

	return solutionGameBoard;
}
