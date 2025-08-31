#include "../include/fileIO.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/utility.h"

/*
 * Opens an input file either from the command-line argument or via user input.
 *
 * - If a filename is not provided via command-line (argc == 1), prompts the user to enter one.
 * - Continues prompting (currently in an infinite loop) until a valid file is opened.
 * - Returns a FILE pointer opened in read mode.
 *
 * TODO: Replace infinite loop with an explicit "Try again?" prompt to allow graceful exit.
 */
FILE * getFile (char * cliFileName)
{
	FILE * fPtr = NULL;
	char fileName[256];
	char loop;

	do {
		loop = FALSE;

		/* Filename not passed as argument when calling the program */
		if (cliFileName == NULL)
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
		else strcpy(fileName, cliFileName);

		/* If filename passed as argument is invalid, need to prompt user next loop iteration */
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
 * Parses a puzzle file to extract the gameboard dimensions and clues.
 *
 * Expected file format:
 * - First line: two integers (width height)
 * - Next (width + height) lines: clues for each row and column, space-separated
 *
 * Parsing details:
 * - Width and height must be integers with no trailing garbage.
 * - Each clue line must contain only valid integers (1–50), separated by spaces.
 * - Ignores trailing whitespace, but rejects any other trailing characters.
 * - Maximum 32 clues per line.
 *
 * Returns:
 * - A pointer to an array of LineClue pointers (rows first, then columns), or NULL on error.
 * - On error, prints a detailed message and frees all allocated memory.
 *
 * TODO: Add support for alternative delimiters beyond space (e.g., tabs or commas).
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
			goto free_Memory;
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
				goto free_Memory;
			}

			offset += numCharsRead;
			++j;
		}

		rest = fileLine + offset;

		/* Looping until the null terminator is reach or a non space character */
		while (*rest != '\0' && isspace((unsigned char) *rest)) ++rest;

		/* If the null terminator is not reached then an invalid character is in the line after the clues */
		if (*rest != '\0')
		{
			fprintf(stderr,"Invalid contents on line %d: trailing characters after clues.\n", fileLineNum);
			goto free_Memory;
		}

		lineClues[i] = createLineClueSet(clueBuffer, j);

		if (lineClues[i] == NULL)
		{
			fprintf(stderr,"Error allocating memory for clueSet on line: %d\n", fileLineNum);
			goto free_Memory;
		}
	}
	
	return lineClues;

free_Memory:
	for (i = i - 1; i >= 0; --i)
	{
		free(lineClues[i]->clues);
		lineClues[i]->clues = NULL;

		free(lineClues[i]);
		lineClues[i] = NULL;
	}

	free(lineClues);
	lineClues = NULL;	
		
	return lineClues;
}
