#include <stdlib.h>
#include <string.h>
#include "../include/gameBoard.h"

#define GENERATION_LIMIT 10000
#define SOLVED_THRESHOLD 0.30

/*
 * TODO: Free SubLine allocated memory.
 * 
 * TODO: Add force generation if no rows/columns to update.
 *
 * TODO: Test for best generation limit, solved threshold and subline max size.
 */

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
int * solvePuzzle (FILE * filePtr, SolvingMode mode, int * iterations, int * totalPermutations, int * maxPermutations, Timings * timings)
{
	/* Zero-initialize all context fields */
	SolverContext solver = {0};
	int i, width = 0, length = 0, loop = 1;

	/* Run 10 times for benchmarking */
	if (mode == MODE_BENCHMARK) loop = 10;

	timingEnd(timings, FILEREADING);
	
	do
	{
		timingStart(timings, FILEREADING);
		/* Initialize puzzle data structures and directly frees memory if an allocation error occurs */
		if (!puzzleSetup(filePtr, &solver, timings))
			goto free;
			
		width = solver.width;
		length = solver.length;

		timingEnd(timings, INIT);
		timingStart(timings, OVERLAP);
		
		for (i = 0; i < length; ++i)
		{
			overlap(solver.lines[i]);
			setGameBoardRow(&solver, i);
			*maxPermutations += solver.lines[i]->maxPermutations;
		}

		for ( ; i < width + length; ++i)
		{
			overlap(solver.lines[i]);
			setGameBoardColumn(&solver, i);
			*maxPermutations += solver.lines[i]->maxPermutations;
		}

		timingEnd(timings, OVERLAP);
		timingStart(timings, SOLVING);

		/* Repeat solving pass until the gameBoard is completely solved */
		while (!isSolved(solver.gameBoard, width, length))
		{
			++(*iterations);

			if (solver.rowsToUpdateBits == 0)
			{
				int j = -1;

				for (i = 0; i < length; ++i)
				{
					if (solver.lines[i]->storeCount == 0)
					{
						if (j == -1)
							j = i;

						else if (solver.lines[i]->maxPermutations < solver.lines[j]->maxPermutations)
							j = i;
					}
				}

				if (j > -1)
				{
					if (!lineGeneration(solver.lines[j], timings, totalPermutations, solver.setBitIndexes))
						goto free;
				}
			}

			else
			{
				do
				{
					i = __builtin_ctzll(solver.rowsToUpdateBits);
					solver.rowsToUpdateBits &= ~(1ULL << i);

					/* Update row BitMasks based on current gameBoard state */
					updateBitMasks(solver.lines[i], solver.gameBoard + (i * width));

					/* Allocate, store and reallocate permutations if not done yet for the row */
					if (solver.lines[i]->storeCount == 0)
					{
						if (!generationDecision(solver.lines[i], timings, totalPermutations, solver.setBitIndexes))
							goto free;
					}

					/* Only filter if permutations were already generated */
					else
					{	
						timingStart(timings, FILTERING);
						filterPermutations(solver.lines[i]);
						timingEnd(timings, FILTERING);

						timingStart(timings, COMMON);
						generateConsistentPattern(solver.lines[i]);
						timingEnd(timings, COMMON);
					}
					
					setGameBoardRow(&solver, i);
					
				} while (solver.rowsToUpdateBits != 0);
				
			}

			if (solver.columnsToUpdateBits == 0)
			{
				int j = -1;

				for (i = length; i < length + width; ++i)
				{
					if (solver.lines[i]->storeCount == 0)
					{
						if (j == -1)
							j = i;

						else if (solver.lines[i]->maxPermutations < solver.lines[j]->maxPermutations)
							j = i;
					}
				}

				if (j > -1)
				{
					if (!lineGeneration(solver.lines[j], timings, totalPermutations, solver.setBitIndexes))
						goto free;
				}
			}

			else
			{
				do
				{
					i = __builtin_ctzll(solver.columnsToUpdateBits);
					solver.columnsToUpdateBits &= ~(1ULL << i);
					i += length;

					/* Retrieve column's current gameBoard state and update the BitMasks based on this */
					getGameBoardColumn(solver.gameBoard, solver.columnPartialSolution, width, length, i - length);
					updateBitMasks(solver.lines[i], solver.columnPartialSolution);

					/* Allocate, store and reallocate permutations if not done yet for the column */
					if (solver.lines[i]->storeCount == 0)
					{
						if (!generationDecision(solver.lines[i], timings, totalPermutations, solver.setBitIndexes))
							goto free;
					}

					/* Only filter if permutations were already generated */
					else
					{	
						timingStart(timings, FILTERING);
						filterPermutations(solver.lines[i]);
						timingEnd(timings, FILTERING);

						timingStart(timings, COMMON);
						generateConsistentPattern(solver.lines[i]);
						timingEnd(timings, COMMON);
					}
					
					setGameBoardColumn(&solver, i);
					
				} while (solver.columnsToUpdateBits != 0);
			}
		}

		if (loop > 1)
		{
			rewind(filePtr);
			freeResources(&solver);
			free(solver.gameBoard);
			solver.gameBoard = NULL;
		}
		
		timingEnd(timings, SOLVING);
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
char puzzleSetup (FILE * filePtr, SolverContext * solver, Timings * timings)
{
	int i, width = 0, length = 0;
	uint64_t maxPermutation = 0;

	solver->width 				  = 0;
	solver->length 		  		  = 0;
	solver->gameBoard 			  = NULL;
	solver->rowsToUpdateBits 	  = 0ULL;
	solver->columnsToUpdateBits   = 0ULL;
	solver->columnPartialSolution = NULL;
	solver->lineClues 			  = NULL;
	solver->lines 				  = NULL;
	solver->stage 				  = FREE_NONE;

	/* Parse clues from file and get puzzle dimensions */
	solver->lineClues = readFile(filePtr, &width, &length);
	if (solver->lineClues == NULL)
		return FALSE;	

	timingEnd(timings, FILEREADING);
	timingStart(timings, INIT);

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

		if (solver->lines[i]->maxPermutations > maxPermutation)
			maxPermutation = solver->lines[i]->maxPermutations;	
	}

	/* Create Line structs for each column */
	for ( ; i < width + length; ++i)
	{
		solver->lines[i] = createLine(solver->lineClues[i], length, i);

		if (solver->lines[i] == NULL)
			return FALSE;

		if (solver->lines[i]->maxPermutations > maxPermutation)
			maxPermutation = solver->lines[i]->maxPermutations;
	}

	solver->setBitIndexes = (int *)malloc(sizeof(int) * maxPermutation);
	if (solver->setBitIndexes == NULL)
		return FALSE;

	solver->stage = FREE_SET_BIT_INDEXES;

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
		case FREE_SET_BIT_INDEXES:
			free(solver->setBitIndexes);
			solver->setBitIndexes = NULL;
			/* Fall through */
			
		case FREE_LINES:
			for (i = 0; i < totalClues; ++i)
			{
				/* Lines are allocated sequentially — a NULL line indicates end of valid data. */
				if (solver->lines[i] == NULL) break;

				if (solver->lines[i]->startEdge != NULL)
					switch (solver->lines[i]->startEdge->state)
					{
						case LINE_ALLOC_ALL:
							free(solver->lines[i]->startEdge->bitSet->words);						
							free(solver->lines[i]->startEdge->bitSet);
							solver->lines[i]->startEdge->bitSet = NULL;
							/* Fall through */

						case LINE_ALLOC_PERMS:
							free(solver->lines[i]->startEdge->permutations);
							solver->lines[i]->startEdge->permutations = NULL;
							/* Fall through */

						case LINE_ALLOC_NONE:
							free(solver->lines[i]->startEdge);
							solver->lines[i]->startEdge = NULL;
							break;
					}

				if (solver->lines[i]->endEdge != NULL)
					switch (solver->lines[i]->endEdge->state)
					{
						case LINE_ALLOC_ALL:
							free(solver->lines[i]->endEdge->bitSet->words);						
							free(solver->lines[i]->endEdge->bitSet);
							solver->lines[i]->endEdge->bitSet = NULL;
							/* Fall through */

						case LINE_ALLOC_PERMS:
							free(solver->lines[i]->endEdge->permutations);
							solver->lines[i]->endEdge->permutations = NULL;
							/* Fall through */

						case LINE_ALLOC_NONE:
							free(solver->lines[i]->endEdge);
							solver->lines[i]->endEdge = NULL;
							break;
					}

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
						free(solver->lines[i]);
						solver->lines[i] = NULL;
						break;
				}
			}

			free(solver->lines);
			solver->lines = NULL;
			/* Fall through */
	
		case FREE_COLUMN_PARTIAL:
			free(solver->columnPartialSolution);
			solver->columnPartialSolution = NULL;
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

char generationDecision (Line * line, Timings * timings, int * totalPermutations, int * setBitIndexes)
{
	uint64_t tempMaskBits = line->maskBits;
	uint64_t tempPartialBits = line->partialBits;
	
	if (line->maxPermutations > GENERATION_LIMIT)
	{
		timingStart(timings, EDGE_DEDUCTION);
	
		if (line->startEdge == NULL)
		{
			line->startEdge = createSubLine(line->clueSet, line->size);
			if (line->startEdge == NULL) return FALSE;

			line->endEdge = createSubLine(line->clueSet, line->size);
			if (line->endEdge == NULL) return FALSE;

			updateSubLineBitMasks(line);

			generateSubLinePermutationsStart(line->startEdge, 0, 0ULL, TRUE, 0, &(line->startEdge->permCount));
			generateSubLinePermutationsEnd(line->endEdge, line->clueSet->clueCount - 1, 0ULL, 1ULL << line->size, TRUE, 0, &(line->endEdge->permCount));
	
			line->startEdge->permutations = (uint64_t *)malloc(sizeof(uint64_t) * line->startEdge->permCount);
			if (line->startEdge->permutations == NULL) 
				return FALSE;

			line->startEdge->state = LINE_ALLOC_PERMS;
			
			line->startEdge->bitSet = newBitSet(line->startEdge->permCount);
			if (line->startEdge->bitSet == NULL)
				return FALSE;

			line->startEdge->state = LINE_ALLOC_ALL;
			
			line->endEdge->permutations = (uint64_t *)malloc(sizeof(uint64_t) * line->endEdge->permCount);
			if (line->endEdge->permutations == NULL) 
				return FALSE;

			line->endEdge->state = LINE_ALLOC_PERMS;
			
			line->endEdge->bitSet = newBitSet(line->endEdge->permCount);
			if (line->endEdge->bitSet == NULL)
				return FALSE;

			line->endEdge->state = LINE_ALLOC_ALL;

			generateSubLinePermutationsStart(line->startEdge, 0, 0ULL, FALSE, 0, &(line->startEdge->storeCount));
			generateSubLinePermutationsEnd(line->endEdge, line->clueSet->clueCount - 1, 0ULL, 1ULL << line->size, FALSE, 0, &(line->endEdge->storeCount));

			*totalPermutations += line->startEdge->storeCount;
			*totalPermutations += line->endEdge->storeCount;
		}

		else
		{
			updateSubLineBitMasks(line);

			filterSubLinePermutations(line);
		}

		generateSubLinesConsistentPattern (line);

		updateBitMasksFromSubLines(line);

		timingEnd(timings, EDGE_DEDUCTION);

		if (tempMaskBits != line->maskBits || tempPartialBits != line->partialBits)
			return TRUE;

		if ( (__builtin_popcountll(line->maskBits) / (float)line->size ) < SOLVED_THRESHOLD)
			return TRUE;	
	}

	return lineGeneration(line, timings, totalPermutations, setBitIndexes);
}

char lineGeneration (Line * line, Timings * timings, int * totalPermutations, int * setBitIndexes)
{
	timingStart(timings, INIT);

	line->permutations = (uint64_t *)malloc(sizeof(uint64_t) * line->maxPermutations);
	if (line->permutations == NULL)
		return FALSE;

	line->state = LINE_ALLOC_PERMS;

	timingEnd(timings, INIT);
	timingStart(timings, GENERATION);

	/* Actually generate and store the permutations */
	generatePermutations(line, &(line->storeCount));

	timingEnd(timings, GENERATION);
	timingStart(timings, INIT);

	line->permutations = (uint64_t *)realloc(line->permutations, sizeof(uint64_t) * line->storeCount);

	line->bitSet = newBitSet(line->storeCount);
	if (line->bitSet == NULL)
		return FALSE;

	line->state = LINE_ALLOC_ALL;

	line->setBitIndexes = setBitIndexes;
		
	*totalPermutations += line->storeCount;

	timingEnd(timings, INIT);

	timingStart(timings, COMMON);
	generateConsistentPattern(line);
	timingEnd(timings, COMMON);

	return TRUE;
}
