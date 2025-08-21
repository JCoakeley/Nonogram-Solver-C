#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "../include/utility.h"
#include "../include/gameBoard.h"
#include "../include/solver.h"

FILE * getFile (int, char *);

LineClue ** readFile (FILE *, int *, int *);

int main (int argc, char ** argv)
{
	struct timespec endTime, startTime;
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	int i, iterations = 0, gameBoardWidth = 0, gameBoardLength = 0;
	FILE * fPtr = NULL;
	LineClue ** lineClues = NULL;
	Line ** lines = NULL;
	int * gameBoard = NULL, * rowsToUpdate = NULL, * columnsToUpdate = NULL;
	int * columnPartialSolution = NULL;

	fPtr = getFile(argc, argv[1]);

	lineClues = readFile(fPtr, &gameBoardWidth, &gameBoardLength);
	fclose(fPtr);
	fPtr = NULL;

	if (lineClues == NULL) return EXIT_FAILURE;

	rowsToUpdate = (int *)malloc(sizeof(int) * gameBoardLength);
	columnsToUpdate = (int *)malloc(sizeof(int) * gameBoardWidth);
	columnPartialSolution = (int *)malloc(sizeof(int) * gameBoardLength);

	gameBoard = createGameBoard(gameBoardWidth, gameBoardLength);
	if (gameBoard == NULL) return EXIT_FAILURE;

	lines = (Line **)malloc(sizeof(Line *) * (gameBoardWidth + gameBoardLength));
	if (lines == NULL) return EXIT_FAILURE;

	for (i = 0; i < gameBoardLength; ++i)
		lines[i] = createLine(lineClues[i], gameBoardWidth, i);

	for ( ; i < gameBoardWidth + gameBoardLength; ++i)
		lines[i] = createLine(lineClues[i], gameBoardLength, i);

	for (i = 0; i < gameBoardWidth + gameBoardLength; ++i)
		generatePermutations(lines[i], 0, 0ULL, 0, TRUE, &(lines[i]->permutationCount));

	for (i = 0; i < gameBoardWidth + gameBoardLength; ++i)
	{
		lines[i]->permutations = (uint64_t *)malloc(sizeof(uint64_t) * lines[i]->permutationCount);
		lines[i]->bitSet = newBitSet(lines[i]->permutationCount);
	}

	for (i = 0; i < gameBoardWidth + gameBoardLength; ++i)
		generatePermutations(lines[i], 0, 0ULL, 0, FALSE, &(lines[i]->storeCount));

	while (!isSolved(gameBoard, gameBoardWidth, gameBoardLength))
	{
		++iterations;
		for (i = 0; i < gameBoardLength; ++i)
		{
			updateBitMasks(lines[i], gameBoard + (i * gameBoardWidth));
			filterPermutations(lines[i]);
			generateConsistentPattern(lines[i]);
			setGameBoardRow(gameBoard, lines[i], columnsToUpdate);
		}
		
		for ( ; i < gameBoardWidth + gameBoardLength; ++i)
		{
			getGameBoardColumn(gameBoard, columnPartialSolution, gameBoardWidth, gameBoardLength, i - gameBoardLength);
			updateBitMasks(lines[i], columnPartialSolution);
			filterPermutations(lines[i]);
			generateConsistentPattern(lines[i]);
			setGameBoardColumn(gameBoard, lines[i], gameBoardWidth, rowsToUpdate);		
		}
	}

	printGameBoard(gameBoard, gameBoardWidth, gameBoardLength);

	clock_gettime(CLOCK_MONOTONIC, &endTime);

	long nanos = (endTime.tv_sec - startTime.tv_sec) * 1000000000L + (endTime.tv_nsec - startTime.tv_nsec);
	
	printf("Time: %ldns, Iterations: %d\n", nanos, iterations);

	return EXIT_SUCCESS;
}

/*
* TODO: Add a try again prompt instead of endlessly asking for a valid
* file name until on is provided.
* 
* If no argument has been passed in when the program is run then asks the
* user for a filename. Opens a file stream to the provided filename and returns
* it. Will loop until a valid filename is provided.
*/
FILE * getFile (int argc, char * argv)
{
	FILE * fPtr = NULL;
	char fileName[256];
	bool loop;

	do {
		loop = FALSE;

		/* Filename not passed as argument when calling the program */
		if (argc == 1)
		{
			printf("Please enter a filename: ");

			if (fgets(fileName, sizeof(fileName), stdin) == NULL)
			{
				fprintf(stderr, "Error reading input\n");
				loop = TRUE;
				continue;
			}

			fileName[strcspn(fileName, "\n")] = '\0';

		    if (fileName[0] == '\0') 
		    {
		        fprintf(stderr,"Filename cannot be empty.\n");
		        loop = TRUE;
		        continue;
		    }
		}

		/* Filename pass as argument when calling the program */
		else strcpy(fileName, argv);

		/* If filename passed as argument is invalid, need to prompt user next loop iteration */
		argc = 1;
		fPtr = fopen(fileName, "r");

		if (fPtr == NULL)
		{
			fprintf(stderr,"Failed to open file: %s\n", fileName);
			loop = TRUE;
		}
	} while (loop);
	
	return fPtr;
}

/*
* TODO: Add support for various delimiters, not just spaces.
* 
* Parses file line by line. First reading the width and length from the first line.
* Then reading each row/column's clue set on each subsequent line storing these clues
* in a buffer until the entire line is read. Handles reaching EOF early, unexpected
* characters, max 32 clues per line.
*/
LineClue ** readFile (FILE * fPtr, int * width, int * length)
{
	char fileLine[256], * rest = NULL;
	int numCharsRead = 0, fileLineNum = 0;
	int i, j, offset = 0;
	int clueBuffer[32];
	LineClue ** lineClues = NULL;

	/* Reading gameboard width and length from file with validation checking */
	if (!fgets(fileLine, sizeof(fileLine), fPtr))
	{
		fprintf(stderr,"Error: Empty file or read error.\n");
		return NULL;
	}
	++fileLineNum;
	
	if (sscanf(fileLine, "%d %d %n", width, length, &numCharsRead) != 2)
	{
		fprintf(stderr,"Invalid contents on line %d: expected two integers (width, length).\n", fileLineNum);
		return NULL;
	}

	rest = fileLine + numCharsRead;

	/* Looping until the null terminator is reach or a non space character */
	while (*rest != '\0' && isspace((unsigned char) *rest)) ++rest;

	/* If the null terminator is not reached then an invalid character is in the line after the width
		and length */
	if (*rest != '\0')
	{
		fprintf(stderr,"Invalid contents on line %d: trailing characters after width/length.\n", fileLineNum);
		return NULL;
	}

	lineClues = (LineClue **)malloc(sizeof(LineClue *) * (*width + *length));

	if (lineClues == NULL)
	{
		fprintf(stderr, "Error allocating memory for references to each lines clues struct\n");
		return NULL;
	}

	/* Reading each line of clues */
	for (i = 0; i < (*width + *length); ++i)
	{		
		if (!fgets(fileLine, sizeof(fileLine), fPtr))
		{
			fprintf(stderr,"Error: EOF unexpectedly reached after line: %d\n", fileLineNum);
			return NULL;
		}
		
		++fileLineNum;
		j = 0;
		offset = 0;

		/* Reading in one integer at a time until a match is not made or buffer limit reached.
			Adds a progressive offset to the string being passed to sscanf  to accomplish reading
			one integer at a time. */
		while (j < 32 && sscanf(fileLine + offset, "%d %n", &clueBuffer[j], &numCharsRead) == 1)
		{ 
			if (clueBuffer[j] < 1 || clueBuffer[j] > 50)
			{
				fprintf(stderr,"Invalid file contents line: %d, Expecting integers between 1 and 50.\n", fileLineNum);
				return NULL;
			}

			offset += numCharsRead;
			++j;
		}

		rest = fileLine + offset;

		/* Looping until the null terminator is reach or a non space character */
		while (*rest != '\0' && isspace((unsigned char) *rest)) ++rest;

		/* If the null terminator is not reached then an invalid character is in the line after the width
			and length */
		if (*rest != '\0')
		{
			fprintf(stderr,"Invalid contents on line %d: trailing characters after clues.\n", fileLineNum);
			return NULL;
		}

		lineClues[i] = createLineClueSet(clueBuffer, j);

		if (lineClues[i] == NULL)
		{
			fprintf(stderr,"Error allocating memory for clueSet on line: %d\n", fileLineNum);
			return NULL;
		}
	}
	
	return lineClues;
}
