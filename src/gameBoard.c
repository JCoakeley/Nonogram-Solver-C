#include "../include/gameBoard.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
* TODO: May directly update a line struct's mask and partial bits from the gameboard
* instead of create an interim array.
*/

/* 
* Creates a gameboard of the specified width and length in contiguous
* memory. Also sets each element to -1 because elements that are not -1
* will not be updated in the update function. Pointer to the integer
* array is returned.
*/
int * createGameBoard (int width, int length)
{
	int * gameBoard = (int *)malloc(sizeof(int) * width * length);

	if (gameBoard == NULL)
		fprintf(stderr, "Error allocating memory for the gameboard\n");

	else
		memset(gameBoard, 0xFF, sizeof(int) * width * length);
	
	return gameBoard;
}

/*
* Prints the gameboard to the terminal using ■ for a filled cell,
* ' ' for an unsolved cell and X for a solved empty cell. The gameboard
* is divided into 5x5 sections for easier readability.
*/
void printGameBoard (int * gameBoard, int width, int length)
{
	int i, j, charWidth = 2 * (width + (width / 5)) + 2;
	char borderLine[123];

	memset(borderLine, '-', charWidth);
	borderLine[0] = ' ';
	borderLine[charWidth] = '\0';

	for (i = 0; i < length; ++i)
	{
		if (i % 5 == 0)
			printf("%s\n", borderLine);

		for (j = 0; j < width; ++j)
		{
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
* Iterates through each element in the specified row of the game board and if the corresponding
* mask bit is set and the element is unsolved (equals -1) then will update it based on if the
* corresponding partial bit is set or not. Tracks which elements are updated in the
* columnsToUpdate array.
*/
void setGameBoardRow (int * gameBoard, Line * line, int * columnsToUpdate)
{
	int i;
	const int width = line->size;
	const uint64_t maskBits = line->maskBits;
	const uint64_t partialBits = line->partialBits;
	uint64_t compareBit = 1ULL;
	
	gameBoard += line->lineId * width;

	for (i = 0; i < width; ++i, compareBit <<= 1)
		if ((compareBit & maskBits) != 0 && gameBoard[i] == -1)
		{
			gameBoard[i] = ((compareBit & partialBits) == 0) ? 0 : 1;
			columnsToUpdate[i] = 1;
		}

	return;
}

/* 
* Iterates through each element in the specified column of the game board and if the corresponding
* mask bit is set and the element is unsolved (equals -1) then will update it based on if the
* corresponding partial bit is set or not. Tracks which elements are updated in the
* rowsToUpdate array.
*/
void setGameBoardColumn (int * gameBoard, Line * line, int width, int * rowsToUpdate)
{
	int i, index = line->lineId - width;
	const int length = line->size;
	const uint64_t maskBits = line->maskBits;
	const uint64_t partialBits = line->partialBits;
	uint64_t compareBit = 1ULL;

	for (i = 0; i < length; ++i, compareBit <<= 1, index += width)
		if ((compareBit & maskBits) != 0 && gameBoard[index] == -1)
		{
			gameBoard[index] = ((compareBit & partialBits) == 0) ? 0 : 1;
			rowsToUpdate[i] = 1;
		}

	return;
}

/*
* TODO: May implement this more directly in updateBitMasks in solver.c.
* 
* Populates a column array for the specified column from the gameboard.
*/
void getGameBoardColumn (int * gameBoard, int * columnArr, int width, int length, int columnNum)
{
	int i, index = columnNum;

	for (i = 0; i < length; ++i, index +=width)
		columnArr[i] = gameBoard[index];

	return;
}

/* 
* Iterates through every element of the game board and as soon as a -1
* stops and returns false. If no -1 is found the game board is solved.
*/
bool isSolved (int * gameBoard, int width, int length)
{
	bool solved = TRUE;
	int i;

	for (i = 0; i < width * length; ++i)
		if (gameBoard[i] == -1)
		{
			solved = FALSE;
			break;
		}
	
	return solved;
}
