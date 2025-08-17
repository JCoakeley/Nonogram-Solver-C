#include "../include/gameBoard.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
		memset(gameBoard, -1, width * length);
	
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
* Iterates through each element in the specified row of the game board and if the element is
* unsolved (equals -1) then will update it if it differs from the matching element in rowArr.
* rowArr is the updated partial solution for that row. Tracks which elements are updated in the
* columnsToUpdate array and returns this.
*/
int * setGameBoardRow (int * gameBoard, int * rowArr, int row, int width, int * columnsToUpdate)
{
	int i;
	gameBoard += (row * width);

	for (i = 0; i < width; ++i)
		if (gameBoard[i] == -1 && rowArr[i] != gameBoard[i])
		{
			gameBoard[i] = rowArr[i];
			columnsToUpdate[i] = 1;
		}

	return columnsToUpdate;
}

/*
* Iterates through each element in the specified column of the game board and if the element is
* unsolved (equals -1) then will update it if it differs from the matching element in columnArr.
* columnArr is the updated partial solution for that column. Tracks which elements are updated in the
* rowsToUpdate array and returns this.
*/
int * setGameBoardColumn (int * gameBoard, int * columnArr, int column, int length, int width, int * rowsToUpdate)
{
	int i, index = 0;

	for (i = 0; i < length; ++i)
	{
		index = (i * width) + column;
		if (gameBoard[index] == -1 && columnArr[i] != gameBoard[index])
		{
			gameBoard[index] = columnArr[i];
			rowsToUpdate[i] = 1;
		}
	}

	return rowsToUpdate;
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
