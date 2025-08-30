#include "../include/solverAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/fileIO.h"
#include "../include/gameBoard.h"

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
	SolverContext solver;
	int i, width = 0, length = 0;

	solver.width = 0;
	solver.length = 0;
	solver.gameBoard = NULL;
	solver.rowsToUpdate = NULL;
	solver.columnsToUpdate = NULL;
	solver.columnPartialSolution = NULL;
	solver.lineClues = NULL;
	solver.lines = NULL;
	solver.stage = FREE_NONE;

	if (mode == 0 || mode == 1)
	{
		solver.lineClues = readFile(filePtr, &width, &length);
		if (solver.lineClues == NULL)
			return NULL;

		solver.width = width;
		solver.length = length;

		solver.rowsToUpdate = (int *)calloc(length, sizeof(int));
		if (solver.rowsToUpdate == NULL)
			goto free;

		solver.stage = FREE_ROWS;
			
		solver.columnsToUpdate = (int *)calloc(width, sizeof(int));
		if (solver.columnsToUpdate == NULL)
			goto free;

		solver.stage = FREE_COLUMNS;
			
		solver.columnPartialSolution = (int *)calloc(length, sizeof(int));
		if (solver.columnPartialSolution == NULL)
			goto free;

		solver.stage = FREE_COLUMN_PARTIAL;

		solver.gameBoard = createGameBoard(width, length);
		if (solver.gameBoard == NULL)
			goto free;

		solver.lines = (Line **)malloc(sizeof(Line *) * (width + length));
		if (solver.lines == NULL)
			goto free;

		for (i = 0; i < length; ++i)
		{
			solver.lines[i] = createLine(solver.lineClues[i], width, i);

			if (solver.lines[i] == NULL)
				goto free;

			if (i == 0)
				solver.stage = FREE_LINES;
		}

		for ( ; i < width + length; ++i)
		{
			solver.lines[i] = createLine(solver.lineClues[i], length, i);

			if (solver.lines[i] == NULL)
				goto free;
		}

		for (i = 0; i < length; ++i)
		{
			overlap(solver.lines[i]);
			setGameBoardRow(solver.gameBoard, solver.lines[i], solver.columnsToUpdate);
		}

		for ( ; i < width + length; ++i)
		{
			overlap(solver.lines[i]);
			setGameBoardColumn(solver.gameBoard, solver.lines[i], width, solver.rowsToUpdate);
		}

		while (!isSolved(solver.gameBoard, width, length))
		{
			++(*iterations);
			for (i = 0; i < length; ++i)
			{
				if (solver.rowsToUpdate[i] == 1)
				{
					updateBitMasks(solver.lines[i], solver.gameBoard + (i * width));

					if (solver.lines[i]->permutationCount == 0)
					{
						generatePermutations(solver.lines[i], 0, 0ULL, 0, TRUE, &(solver.lines[i]->permutationCount));

						solver.lines[i]->permutations = (uint64_t *)malloc(sizeof(uint64_t) * solver.lines[i]->permutationCount);
						if (solver.lines[i]->permutations == NULL)
							goto free;

						solver.lines[i]->state = LINE_ALLOC_PERMS;
							
						solver.lines[i]->bitSet = newBitSet(solver.lines[i]->permutationCount);
						if (solver.lines[i]->bitSet == NULL)
							goto free;

						solver.lines[i]->state = LINE_ALLOC_ALL;

						generatePermutations(solver.lines[i], 0, 0ULL, 0, FALSE, &(solver.lines[i]->storeCount));
					}

					else
						filterPermutations(solver.lines[i]);
					
					generateConsistentPattern(solver.lines[i]);
					setGameBoardRow(solver.gameBoard, solver.lines[i], solver.columnsToUpdate);
				}
			}

			memset(solver.rowsToUpdate, 0x00, sizeof(int) * length);


			/* TODO: update memory freeing since memory is allocated through the solve and not sequencially */
			for ( ; i < width + length; ++i)
			{
				if (solver.columnsToUpdate[i - length] == 1)
				{
					getGameBoardColumn(solver.gameBoard, solver.columnPartialSolution, width, length, i - length);
					updateBitMasks(solver.lines[i], solver.columnPartialSolution);

					if (solver.lines[i]->permutationCount == 0)
					{
						generatePermutations(solver.lines[i], 0, 0ULL, 0, TRUE, &(solver.lines[i]->permutationCount));

						solver.lines[i]->permutations = (uint64_t *)malloc(sizeof(uint64_t) * solver.lines[i]->permutationCount);
						if (solver.lines[i]->permutations == NULL)
							goto free;

						solver.lines[i]->state = LINE_ALLOC_PERMS;
							
						solver.lines[i]->bitSet = newBitSet(solver.lines[i]->permutationCount);
						if (solver.lines[i]->bitSet == NULL)
							goto free;

						solver.lines[i]->state = LINE_ALLOC_ALL;

						generatePermutations(solver.lines[i], 0, 0ULL, 0, FALSE, &(solver.lines[i]->storeCount));
					}

					else
						filterPermutations(solver.lines[i]);
					
					generateConsistentPattern(solver.lines[i]);
					setGameBoardColumn(solver.gameBoard, solver.lines[i], width, solver.rowsToUpdate);
				}		
			}

			memset(solver.columnsToUpdate, 0x00, sizeof(int) * width);
		}
	}

	if (mode == 0)
		printGameBoard(solver.gameBoard, width, length);

free:
	freeResources(&solver);
	
	return solver.gameBoard;
}

void freeResources (SolverContext * solver)
{
	int i, totalClues = solver->width + solver->length;
	
	switch (solver->stage)
	{
		case FREE_LINES:
			for (i = 0; i < totalClues; ++i)
			{
				if (solver->lines[i] == NULL) break;

				switch (solver->lines[i]->state)
				{
					case LINE_ALLOC_ALL:
						free(solver->lines[i]->bitSet->words);						
						free(solver->lines[i]->bitSet);
						solver->lines[i]->bitSet = NULL;
						/* Fall through */
				
					case LINE_ALLOC_PERMS:
						free(solver->lines[i]->permutations);
						solver->lines[i]->permutations = NULL;
						/* Fall through */
				
					case LINE_ALLOC_NONE:
						break;
				}

				free(solver->lines[i]);
				solver->lines[i] = NULL;
			}

			free(solver->lines);
			solver->lines = NULL;
			/* Fall through */
	
		case FREE_COLUMN_PARTIAL:
			free(solver->columnPartialSolution);
			solver->columnPartialSolution = NULL;
			/* Fall through */
	
		case FREE_COLUMNS:
			free(solver->columnsToUpdate);
			solver->columnsToUpdate = NULL;
			/* Fall through */
	
		case FREE_ROWS:
			free(solver->rowsToUpdate);
			solver->rowsToUpdate = NULL;
			/* Fall through */
	
		case FREE_LINE_CLUES:
			for (i = 0; i < totalClues; ++i)
			{
				if (solver->lineClues[i])
				{
					free(solver->lineClues[i]->clues);				
					free(solver->lineClues[i]);
					solver->lineClues[i] = NULL;
				}
			}

			free(solver->lineClues);
			solver->lineClues = NULL;
			/* Fall through */
	
		case FREE_NONE:
			break;
	}

	return;
}
