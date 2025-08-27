#include "test-FullPuzzles.h"
#include <stdlib.h>
#include "../include/gameBoard.h"
#include "../include/solverAPI.h"

#define TEST1 "tests/testPuzzles/test-10x10.txt"
#define TEST1_SOLUTION "tests/testPuzzles/test-10x10-solution.txt"
#define TEST2 "tests/testPuzzles/test-20x20.txt"
#define TEST2_SOLUTION "tests/testPuzzles/test-20x20-solution.txt"
#define TEST3 "tests/testPuzzles/test-30x30.txt"
#define TEST3_SOLUTION "tests/testPuzzles/test-30x30-solution.txt"
#define TEST4 "tests/testPuzzles/test-50x50.txt"
#define TEST4_SOLUTION "tests/testPuzzles/test-50x50-solution.txt"

int test_fullPuzzles ()
{
	int failures = 0;

	int * expectedSolution = NULL, * actualSolution = NULL, iterations, difference;

	/* Test 1 10x10 Puzzle */
	FILE * filePtr = fopen(TEST1, "r");

	if (filePtr == NULL)
		return -1;

	actualSolution = solvePuzzle(filePtr, 1, &iterations);
	fclose(filePtr);

	filePtr = fopen(TEST1_SOLUTION, "r");
	if (filePtr == NULL)
		return -1;
	
	expectedSolution = generateSolutionGameBoard(filePtr, 10, 10);
	difference = compareGameBoards(expectedSolution, actualSolution, 100);

	free(expectedSolution);
	expectedSolution = NULL;

	free(actualSolution);
	actualSolution = NULL;
	
	fclose(filePtr);
	filePtr = NULL;

	if (difference != 0)
	{
		printf("Test Failure: test-FullPuzzles #1: 10x10\n");
		++failures;
	}

	/* Test 2 20x20 Puzzle */
	filePtr = fopen(TEST2, "r");

	if (filePtr == NULL)
		return -1;

	actualSolution = solvePuzzle(filePtr, 1, &iterations);
	fclose(filePtr);

	filePtr = fopen(TEST2_SOLUTION, "r");
	if (filePtr == NULL)
		return -1;
	
	expectedSolution = generateSolutionGameBoard(filePtr, 20, 20);
	difference = compareGameBoards(expectedSolution, actualSolution, 400);

	free(expectedSolution);
	expectedSolution = NULL;

	free(actualSolution);
	actualSolution = NULL;
	
	fclose(filePtr);
	filePtr = NULL;

	if (difference != 0)
	{
		printf("Test Failure: test-FullPuzzles #2: 20x20\n");
		++failures;
	}

	/* Test 3 30x30 Puzzle */
	filePtr = fopen(TEST3, "r");

	if (filePtr == NULL)
		return -1;

	actualSolution = solvePuzzle(filePtr, 1, &iterations);
	fclose(filePtr);

	filePtr = fopen(TEST3_SOLUTION, "r");
	if (filePtr == NULL)
		return -1;
	
	expectedSolution = generateSolutionGameBoard(filePtr, 30, 30);
	difference = compareGameBoards(expectedSolution, actualSolution, 900);

	free(expectedSolution);
	expectedSolution = NULL;

	free(actualSolution);
	actualSolution = NULL;
	
	fclose(filePtr);
	filePtr = NULL;

	if (difference != 0)
	{
		printf("Test Failure: test-FullPuzzles #3: 30x30\n");
		++failures;
	}

	/* Test 4 50x50 Puzzle */
	filePtr = fopen(TEST4, "r");

	if (filePtr == NULL)
		return -1;

	actualSolution = solvePuzzle(filePtr, 1, &iterations);
	fclose(filePtr);

	filePtr = fopen(TEST4_SOLUTION, "r");
	if (filePtr == NULL)
		return -1;
	
	expectedSolution = generateSolutionGameBoard(filePtr, 50, 50);
	difference = compareGameBoards(expectedSolution, actualSolution, 2500);

	free(expectedSolution);
	expectedSolution = NULL;

	free(actualSolution);
	actualSolution = NULL;
	
	fclose(filePtr);
	filePtr = NULL;

	if (difference != 0)
	{
		printf("Test Failure: test-FullPuzzles #4: 50x50\n");
		++failures;
	}

	return failures;
}

int * generateSolutionGameBoard (FILE * filePtr, int width, int length)
{
	
	int character, index = 0;
	int * solutionGameBoard = createGameBoard (width, length);
	
	if (solutionGameBoard == NULL)
	{
		printf ("error\n");
		return NULL;
	}

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

int compareGameBoards (int * expected, int * actual, int size)
{
	int i, difference = 0;

	for (i = 0; i < size; ++i)
	{
		difference += expected[i] - actual[i];

		if (difference != 0)
			break;
	}

	return difference;
}
