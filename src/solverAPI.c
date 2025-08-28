#include "../include/solverAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/fileIO.h"
#include "../include/gameBoard.h"
#include "../include/solver.h"

/*
* 
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
