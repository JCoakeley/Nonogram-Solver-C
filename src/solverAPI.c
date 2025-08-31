#include "../include/solverAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/fileIO.h"
#include "../include/gameBoard.h"

/*
 * Main puzzle solving entry point.
 *
 * Based on the selected mode, this function sets up the puzzle, solves it,
 * and optionally runs it multiple times for benchmarking.
 *
 * Modes:
 *   MODE_TEST       – solve without printing the board
 *   MODE_STANDARD   – solve and display the board
 *   MODE_BENCHMARK  – solve 10 times to gather averaged performance data
 *
 * Uses helper functions:
 *   - puzzleSetup() for allocation and initialization
 *   - freeResources() for structured cleanup on error or exit
 *
 * Parameters:
 *   filePtr    - open file containing puzzle input
 *   mode       - solving mode (test, display, benchmark)
 *   iterations - pointer to iteration counter, updated during solving
 *
 * Returns:
 *   Pointer to the final game board (dynamically allocated).
 *   NULL if setup or solving failed.
 *
 * Caller is responsible for freeing the returned game board.
 */
int * solvePuzzle (FILE * filePtr, SolvingMode mode, int * iterations)
{
	/* Zero-initialize all context fields */
	SolverContext solver = {0};
	int i, width = 0, length = 0, loop = 1;

	/* Run 10 times for benchmarking */
	if (mode == MODE_BENCHMARK) loop = 10;

	do
	{
		/* Initialize puzzle data structures and directly frees memory if an allocation error occurs */
		if (!puzzleSetup(filePtr, &solver))
			goto free;
			
		width = solver.width;
		length = solver.length;
		
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

		/* Repeat solving pass until the gameBoard is completely solved */
		while (!isSolved(solver.gameBoard, width, length))
		{
			++(*iterations);

			/* --- Row solving pass --- */
			for (i = 0; i < length; ++i)
			{
				if (solver.rowsToUpdate[i] == 1)
				{
					/* Update row BitMasks based on current gameBoard state */
					updateBitMasks(solver.lines[i], solver.gameBoard + (i * width));

					/* Count, allocate and store permutations if not done yet for the row */
					if (solver.lines[i]->permutationCount == 0)
					{
						/* Count permutations first to allocate exact size */
						generatePermutations(solver.lines[i], 0, 0ULL, 0, TRUE, &(solver.lines[i]->permutationCount));

						solver.lines[i]->permutations = (uint64_t *)malloc(sizeof(uint64_t) * solver.lines[i]->permutationCount);
						if (solver.lines[i]->permutations == NULL)
							goto free;

						solver.lines[i]->state = LINE_ALLOC_PERMS;
							
						solver.lines[i]->bitSet = newBitSet(solver.lines[i]->permutationCount);
						if (solver.lines[i]->bitSet == NULL)
							goto free;

						solver.lines[i]->state = LINE_ALLOC_ALL;

						/* Actually generate and store the permutations */
						generatePermutations(solver.lines[i], 0, 0ULL, 0, FALSE, &(solver.lines[i]->storeCount));
					}

					/* Only filter if permutations were already generated */
					else
						filterPermutations(solver.lines[i]);
					
					generateConsistentPattern(solver.lines[i]);
					setGameBoardRow(solver.gameBoard, solver.lines[i], solver.columnsToUpdate);
				}
			}

			/* Reset row update flags for next pass */
			memset(solver.rowsToUpdate, 0x00, sizeof(int) * length);

			/* --- Column solving pass --- */
			for ( ; i < width + length; ++i)
			{
				if (solver.columnsToUpdate[i - length] == 1)
				{
					/* Retrieve column's current gameBoard state and update the BitMasks based on this */
					getGameBoardColumn(solver.gameBoard, solver.columnPartialSolution, width, length, i - length);
					updateBitMasks(solver.lines[i], solver.columnPartialSolution);

					/* Count, allocate and store permutations if not done yet for the column */
					if (solver.lines[i]->permutationCount == 0)
					{
						/* Count permutations first to allocate exact size */
						generatePermutations(solver.lines[i], 0, 0ULL, 0, TRUE, &(solver.lines[i]->permutationCount));

						solver.lines[i]->permutations = (uint64_t *)malloc(sizeof(uint64_t) * solver.lines[i]->permutationCount);
						if (solver.lines[i]->permutations == NULL)
							goto free;

						solver.lines[i]->state = LINE_ALLOC_PERMS;
							
						solver.lines[i]->bitSet = newBitSet(solver.lines[i]->permutationCount);
						if (solver.lines[i]->bitSet == NULL)
							goto free;

						solver.lines[i]->state = LINE_ALLOC_ALL;

						/* Actually generate and store the permutations */
						generatePermutations(solver.lines[i], 0, 0ULL, 0, FALSE, &(solver.lines[i]->storeCount));
					}

					/* Only filter if permutations were already generated */
					else
						filterPermutations(solver.lines[i]);
					
					generateConsistentPattern(solver.lines[i]);
					setGameBoardColumn(solver.gameBoard, solver.lines[i], width, solver.rowsToUpdate);
				}		
			}

			/* Reset column update flags for next pass */
			memset(solver.columnsToUpdate, 0x00, sizeof(int) * width);
		}

		rewind(filePtr);
	} while (--loop > 0);

	/* Only print gameBoard if not in test mode */
	if (mode != MODE_TEST)
		printGameBoard(solver.gameBoard, width, length);

	/* Fress all partially or fully allocated resources */
free:
	freeResources(&solver);
	
	return solver.gameBoard;
}

/*
 * Allocates and initializes all data structures required for solving the puzzle.
 *
 * On success, the SolverContext struct will contain initialized game state, clues,
 * and data structures for tracking progress. Each allocation stage is tracked via
 * the `stage` field to allow partial cleanup if an allocation fails.
 *
 * Parameters:
 *   filePtr - Pointer to an open puzzle file to read.
 *   solver  - Pointer to a SolverContext struct to initialize.
 *
 * Returns:
 *   TRUE  if all allocations succeeded.
 *   FALSE if any allocation failed — caller must call freeResources().
 */
bool puzzleSetup (FILE * filePtr, SolverContext * solver)
{
	int i, width = 0, length = 0;

	solver->width = 0;
	solver->length = 0;
	solver->gameBoard = NULL;
	solver->rowsToUpdate = NULL;
	solver->columnsToUpdate = NULL;
	solver->columnPartialSolution = NULL;
	solver->lineClues = NULL;
	solver->lines = NULL;
	solver->stage = FREE_NONE;

	/* Parse clues from file and get puzzle dimensions */
	solver->lineClues = readFile(filePtr, &width, &length);
	if (solver->lineClues == NULL)
		return FALSE;	

	/* Allocate row update tracking */
	solver->rowsToUpdate = (int *)calloc(length, sizeof(int));
	if (solver->rowsToUpdate == NULL)
		return FALSE;

	solver->stage = FREE_ROWS;

	/* Allocate column update tracking */
	solver->columnsToUpdate = (int *)calloc(width, sizeof(int));
	if (solver->columnsToUpdate == NULL)
		return FALSE;

	solver->stage = FREE_COLUMNS;

	/* Allocate temporary buffer to hold a column's current state */
	solver->columnPartialSolution = (int *)calloc(length, sizeof(int));
	if (solver->columnPartialSolution == NULL)
		return FALSE;

	solver->stage = FREE_COLUMN_PARTIAL;

	/* Allocate the gameBoard (initialized to unsolved state) */
	solver->gameBoard = createGameBoard(width, length);
	if (solver->gameBoard == NULL)
		return FALSE;

	/* Allocate an array of pointers to Line structs (rows + columns) */
	solver->lines = (Line **)malloc(sizeof(Line *) * (width + length));
	if (solver->lines == NULL)
		return FALSE;

	/* Create Line structs for each row */
	solver->stage = FREE_LINES;
	for (i = 0; i < length; ++i)
	{
		solver->lines[i] = createLine(solver->lineClues[i], width, i);

		if (solver->lines[i] == NULL)
			return FALSE;			
	}

	/* Create Line structs for each column */
	for ( ; i < width + length; ++i)
	{
		solver->lines[i] = createLine(solver->lineClues[i], length, i);

		if (solver->lines[i] == NULL)
			return FALSE;
	}

	/* Finalize width and length now that all allocations succeeded */
	solver->width = width;
	solver->length = length;

	return TRUE;
}

/*
 * Frees all dynamically allocated memory tracked by the SolverContext struct.
 *
 * This function uses a staged cleanup mechanism based on the 'stage' enum in
 * SolverContext to ensure only successfully allocated memory is freed. This
 * allows safe cleanup even when solvePuzzle exits early due to allocation failure.
 *
 * Each case in the switch corresponds to a specific allocation stage and falls
 * through to ensure all lower-priority resources are also freed. For example,
 * if stage == FREE_COLUMNS, then columns, rows, and line clues will be freed.
 *
 * Additionally, each Line object maintains its own allocation state to track
 * whether permutations and/or bitSet memory were allocated, enabling partial
 * cleanup of individual lines.
 *
 * Parameters:
 *     solver - A pointer to a SolverContext struct whose members should be freed.
 *
 * Returns:
 *     Nothing. All freed pointers are set to NULL to prevent dangling access.
 */
void freeResources (SolverContext * solver)
{
	int i, totalClues = solver->width + solver->length;
	
	switch (solver->stage)
	{
		case FREE_LINES:
			for (i = 0; i < totalClues; ++i)
			{
				/* Lines are allocated sequentially — a NULL line indicates end of valid data. */
				if (solver->lines[i] == NULL) break;

				/* Free line-specific allocations based on what succeeded */
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
