#include "../include/gameBoard.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * TODO: May directly update a line struct's mask and partial bits from the gameboard
 * instead of create an interim array.
 */

/*
 * Creates and returns a gameboard of the given width and length as a flat int array.
 *
 * All cells are initialized to -1 (unsolved), which is required for the update functions
 * to correctly overwrite only unsolved cells.
 *
 * Returns:
 * - Pointer to the allocated gameboard, or NULL if allocation fails.
 */
int * createGameBoard (int width, int length)
{
	int * gameBoard = (int *)malloc(sizeof(int) * width * length);

	if (gameBoard == NULL)
		fprintf(stderr, "Error allocating memory for the gameboard\n");

	/* Fill all cells with -1 (0xFF) to indicate unsolved state */
	else
		memset(gameBoard, 0xFF, sizeof(int) * width * length);
	
	return gameBoard;
}

/*
 * Prints a formatted view of the gameboard to the terminal.
 *
 * Cell representations:
 * - '■' for filled cells (1)
 * - 'X' for empty cells (0)
 * - ' ' for unsolved cells (-1)
 *
 * The board is grouped into 5×5 visual blocks for readability.
 */
void printGameBoard (int * gameBoard, int width, int length)
{
	int i, j;

	/* charWidth accounts for cell spacing and grid lines */
	int charWidth = 2 * (width + (width / 5)) + 2;
	char borderLine[123];

	/* Create horizontal seperator and null terminate so expected width is printed */
	memset(borderLine, '-', charWidth);
	borderLine[0] = ' ';
	borderLine[charWidth] = '\0';

	for (i = 0; i < length; ++i)
	{
		/* Draw horizontal separator every 5 rows */
		if (i % 5 == 0)
			printf("%s\n", borderLine);

		for (j = 0; j < width; ++j)
		{
			/* Draw vertical separator every 5 columns */
			if (j % 5 == 0)
				printf(" |");

			if (gameBoard[j + (i * width)] == 1)
				printf(" ■");
			else if (gameBoard[j + (i * width)] == 0)
				printf(" X");
			else
				printf("  ");
		}
		printf(" |\n");
	}
	printf("%s\n", borderLine);
	
	return;
}

/*
 * Updates a single row of the gameboard based on the line's maskBits and partialBits.
 *
 * For each bit set in maskBits:
 * - If the corresponding gameboard cell is unsolved (-1), it is set to 1 (filled) or 0 (empty)
 *   depending on the matching bit in partialBits.
 * - The corresponding column is marked as updated in columnsToUpdate[].
 */
void setGameBoardRow (int * gameBoard, Line * line, int * columnsToUpdate)
{
	int i;
	const int width = line->size;
	const uint64_t maskBits = line->maskBits;
	const uint64_t partialBits = line->partialBits;
	uint64_t compareBit = 1ULL;

	/* Move pointer to start of target row */
	gameBoard += line->lineId * width;

	for (i = 0; i < width; ++i, compareBit <<= 1)
	{
		/* If this bit is marked as solved and the cell in the gameBoard is unsolved */
		if ((compareBit & maskBits) != 0 && gameBoard[i] == -1)
		{
			gameBoard[i] = ((compareBit & partialBits) == 0) ? 0 : 1;
			columnsToUpdate[i] = 1;
		}
	}

	return;
}

/*
 * Updates a single column of the gameboard based on the line's maskBits and partialBits.
 *
 * For each bit set in maskBits:
 * - If the corresponding gameboard cell is unsolved (-1), it is set to 1 (filled) or 0 (empty)
 *   depending on the matching bit in partialBits.
 * - The corresponding row is marked as updated in rowsToUpdate[].
 */
void setGameBoardColumn (int * gameBoard, Line * line, int width, int * rowsToUpdate)
{
	int i, index = line->lineId - width;
	const int length = line->size;
	const uint64_t maskBits = line->maskBits;
	const uint64_t partialBits = line->partialBits;
	uint64_t compareBit = 1ULL;

	for (i = 0; i < length; ++i, compareBit <<= 1, index += width)
	{
		/* If this bit is marked as solved and the cell in the gameBoard is unsolved */
		if ((compareBit & maskBits) != 0 && gameBoard[index] == -1)
		{
			gameBoard[index] = ((compareBit & partialBits) == 0) ? 0 : 1;
			rowsToUpdate[i] = 1;
		}
	}

	return;
}

/*
 * Extracts a column from the gameboard into a linear array.
 *
 * This is useful for building interim column arrays for solving or deduction logic.
 * 
 * TODO: Consider inlining or merging into updateBitMasks() for performance or clarity.
 */
void getGameBoardColumn (int * gameBoard, int * columnArr, int width, int length, int columnNum)
{
	int i, index = columnNum;

	/* Traverse down the column, one row at a time */
	for (i = 0; i < length; ++i, index +=width)
		columnArr[i] = gameBoard[index];

	return;
}

/*
 * Checks whether the gameboard is fully solved.
 *
 * Returns TRUE if all cells are non-negative (i.e., no unsolved -1 cells remain),
 * otherwise returns FALSE.
 */
bool isSolved (int * gameBoard, int width, int length)
{
	bool solved = TRUE;
	int i;

	for (i = 0; i < width * length; ++i)
		/* Found an unsolved cell, puzzle not complete */
		if (gameBoard[i] == -1)
		{
			solved = FALSE;
			break;
		}
	
	return solved;
}
