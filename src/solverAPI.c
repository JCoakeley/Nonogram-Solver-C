#include "../include/solverAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/fileIO.h"
#include "../include/gameBoard.h"
#include "../include/solver.h"

/*
 * Solves a Nonogram puzzle from a file input stream and returns the completed game board.
 *
 * Parameters:
 * - filePtr     : FILE pointer to a valid puzzle input file.
 * - mode        : 0 for printing and returning the solution, 1 for benchmarking only (no printing).
 * - iterations  : Pointer to an integer that will be updated with the number of iterations required.
 *
 * Workflow:
 * - Reads the puzzle dimensions and clues using readFile().
 * - Allocates and initializes tracking arrays and the game board.
 * - Creates Line structures for each row and column and applies initial overlap deduction.
 * - Alternates solving rows and columns until the board is fully solved.
 * - For each line, generates permutations only when needed and uses bitmasks to track filtered permutations.
 * - Tracks and reuses consistent patterns to deduce newly solvable cells.
 *
 * Memory Handling:
 * - Allocates memory progressively based on solving needs.
 * - Includes multi-stage cleanup using `goto` and reverse deallocation for safe unwinding.
 * - Frees all intermediate allocations and LineClue structures before returning the final game board.
 *
 * Return:
 * - A pointer to a dynamically allocated, fully solved game board array (int *), or NULL on error.
 *
 * TODO:
 * - Refactor memory cleanup to better match the actual flow and dependencies of allocations.
 * - Consider breaking this function into smaller helpers for readability and maintenance.
 */
int * solvePuzzle (FILE * filePtr, char mode, int * iterations)
{
	int i, j = 0, width = 0, length = 0;
	LineClue ** lineClues = NULL;
	Line ** lines = NULL;
	int * gameBoard = NULL, * rowsToUpdate = NULL, * columnsToUpdate = NULL;
	int * columnPartialSolution = NULL;

	if (mode == 0 || mode == 1)
	{
		lineClues = readFile(filePtr, &width, &length);
		if (lineClues == NULL)
			return NULL;

		rowsToUpdate = (int *)calloc(length, sizeof(int));
		if (rowsToUpdate == NULL)
			goto row_Free;
			
		columnsToUpdate = (int *)calloc(width, sizeof(int));
		if (columnsToUpdate == NULL)
			goto column_Free;
			
		columnPartialSolution = (int *)calloc(length, sizeof(int));
		if (columnPartialSolution == NULL)
			goto partial_Free;

		gameBoard = createGameBoard(width, length);
		if (gameBoard == NULL)
			goto gameBoard_Free;

		lines = (Line **)malloc(sizeof(Line *) * (width + length));
		if (lines == NULL)
			goto lines_Free;

		for (i = 0; i < length; ++i)
		{
			lines[i] = createLine(lineClues[i], width, i);

			if (lines[i] == NULL)
				goto createLine_Free;
		}

		for ( ; i < width + length; ++i)
		{
			lines[i] = createLine(lineClues[i], length, i);

			if (lines[i] == NULL)
				goto createLine_Free;
		}

		for (i = 0; i < length; ++i)
		{
			overlap(lines[i]);
			setGameBoardRow(gameBoard, lines[i], columnsToUpdate);
		}

		for ( ; i < width + length; ++i)
		{
			overlap(lines[i]);
			setGameBoardColumn(gameBoard, lines[i], width, rowsToUpdate);
		}

		while (!isSolved(gameBoard, width, length))
		{
			++(*iterations);
			for (i = 0; i < length; ++i)
			{
				if (rowsToUpdate[i] == 1)
				{
					updateBitMasks(lines[i], gameBoard + (i * width));

					if (lines[i]->permutationCount == 0)
					{
						generatePermutations(lines[i], 0, 0ULL, 0, TRUE, &(lines[i]->permutationCount));

						lines[i]->permutations = (uint64_t *)malloc(sizeof(uint64_t) * lines[i]->permutationCount);
						if (lines[i]->permutations == NULL)
							goto permutation_Free;
							
						lines[i]->bitSet = newBitSet(lines[i]->permutationCount);
						if (lines[i]->bitSet == NULL)
							goto bitSet_Free;

						generatePermutations(lines[i], 0, 0ULL, 0, FALSE, &(lines[i]->storeCount));
					}

					else
						filterPermutations(lines[i]);
					
					generateConsistentPattern(lines[i]);
					setGameBoardRow(gameBoard, lines[i], columnsToUpdate);
				}
			}

			memset(rowsToUpdate, 0x00, sizeof(int) * length);


			/* TODO: update memory freeing since memory is allocated through the solve and not sequencially */
			for ( ; i < width + length; ++i)
			{
				if (columnsToUpdate[i - length] == 1)
				{
					getGameBoardColumn(gameBoard, columnPartialSolution, width, length, i - length);
					updateBitMasks(lines[i], columnPartialSolution);

					if (lines[i]->permutationCount == 0)
					{
						generatePermutations(lines[i], 0, 0ULL, 0, TRUE, &(lines[i]->permutationCount));

						lines[i]->permutations = (uint64_t *)malloc(sizeof(uint64_t) * lines[i]->permutationCount);
						if (lines[i]->permutations == NULL)
							goto permutation_Free;
							
						lines[i]->bitSet = newBitSet(lines[i]->permutationCount);
						if (lines[i]->bitSet == NULL)
							goto bitSet_Free;

						generatePermutations(lines[i], 0, 0ULL, 0, FALSE, &(lines[i]->storeCount));
					}

					else
						filterPermutations(lines[i]);
					
					generateConsistentPattern(lines[i]);
					setGameBoardColumn(gameBoard, lines[i], width, rowsToUpdate);
				}		
			}

			memset(columnsToUpdate, 0x00, sizeof(int) * width);
		}
	}

	if (mode == 0)
		printGameBoard(gameBoard, width, length);
	
	for (i = 0; i < width + length; ++i)
	{
		free(lineClues[i]->clues);
		lineClues[i]->clues = NULL;
		lines[i]->clueSet->clues = NULL;
		free(lineClues[i]);
		lineClues[i] = NULL;
		lines[i]->clueSet = NULL;
	}

	free(lineClues);
	lineClues = NULL;

bitSet_Free:
	if (i < width + length)
	{
		free(lines[i]->permutations);
		lines[i]->permutations = NULL;
	}

permutation_Free:	
	j = i - 1;
	i = width + length;
	for ( ; j >= 0; --j)
	{
		free(lines[j]->bitSet->words);
		lines[j]->bitSet->words = NULL;
		
		free(lines[j]->bitSet);
		lines[j]->bitSet = NULL;

		free(lines[j]->permutations);
		lines[j]->permutations = NULL;
	}

createLine_Free:
	for (i = i - 1; i >= 0 ; --i)
	{
		free(lines[i]);
		lines[i] = NULL;
	}

	free(lines);
	lines = NULL;
lines_Free:

gameBoard_Free:
	
	free(columnPartialSolution);
	columnPartialSolution = NULL;
partial_Free:
	
	free(columnsToUpdate);
	columnsToUpdate = NULL;
column_Free:

	free(rowsToUpdate);
	rowsToUpdate = NULL;
row_Free:
		

	return gameBoard;
}
