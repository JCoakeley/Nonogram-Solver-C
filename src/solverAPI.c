#include "../include/solverAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/fileIO.h"
#include "../include/gameBoard.h"
#include "../include/solver.h"

int * solvePuzzle (FILE * filePtr, char mode, int * iterations)
{
	int i, width = 0, length = 0;
	LineClue ** lineClues = NULL;
	Line ** lines = NULL;
	int * gameBoard = NULL, * rowsToUpdate = NULL, * columnsToUpdate = NULL;
	int * columnPartialSolution = NULL;

	if (mode == 0 || mode == 1)
	{
		lineClues = readFile(filePtr, &width, &length);

		rowsToUpdate = (int *)malloc(sizeof(int) * length);
		columnsToUpdate = (int *)malloc(sizeof(int) * width);
		columnPartialSolution = (int *)malloc(sizeof(int) * length);

		gameBoard = createGameBoard(width, length);

		lines = (Line **)malloc(sizeof(Line *) * (width + length));

		for (i = 0; i < length; ++i)
		lines[i] = createLine(lineClues[i], width, i);

		for ( ; i < width + length; ++i)
			lines[i] = createLine(lineClues[i], length, i);

		for (i = 0; i < width + length; ++i)
			generatePermutations(lines[i], 0, 0ULL, 0, TRUE, &(lines[i]->permutationCount));

		for (i = 0; i < width + length; ++i)
		{
			lines[i]->permutations = (uint64_t *)malloc(sizeof(uint64_t) * lines[i]->permutationCount);
			lines[i]->bitSet = newBitSet(lines[i]->permutationCount);
		}

		for (i = 0; i < width + length; ++i)
			generatePermutations(lines[i], 0, 0ULL, 0, FALSE, &(lines[i]->storeCount));

		while (!isSolved(gameBoard, width, length))
		{
			++(*iterations);
			for (i = 0; i < length; ++i)
			{
				updateBitMasks(lines[i], gameBoard + (i * width));
				filterPermutations(lines[i]);
				generateConsistentPattern(lines[i]);
				setGameBoardRow(gameBoard, lines[i], columnsToUpdate);
			}
			
			for ( ; i < width + length; ++i)
			{
				getGameBoardColumn(gameBoard, columnPartialSolution, width, length, i - length);
				updateBitMasks(lines[i], columnPartialSolution);
				filterPermutations(lines[i]);
				generateConsistentPattern(lines[i]);
				setGameBoardColumn(gameBoard, lines[i], width, rowsToUpdate);		
			}
		}
	}

	if (mode == 0)
		printGameBoard(gameBoard, width, length);

	return gameBoard;
}
