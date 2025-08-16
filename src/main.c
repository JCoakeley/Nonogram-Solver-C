#include <string.h>
#include <ctype.h>
#include "../include/main.h"

int main (int argc, char ** argv)
{
	int gameBoardWidth = 0, gameBoardLength = 0;
	FILE * fPtr = NULL;
	LineClue ** lineClues = NULL;

	fPtr = getFile(argc, argv[1]);

	lineClues = readFile(fPtr, &gameBoardWidth, &gameBoardLength);

	if (lineClues == NULL) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

FILE * getFile (int argc, char * argv)
{
	FILE * fPtr = NULL;
	char fileName[256];
	bool loop;

	do {
		loop = FALSE;
		
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

		else strcpy(fileName, argv);

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
		fprintf(stderr,"Invalid contents on line %d: expected two integers (width length).\n", fileLineNum);
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

	printf("Width: %d\nLength: %d\n", *width, *length);

	lineClues = (LineClue **)malloc(sizeof(LineClue *) * (*width + *length));

	if (lineClues == NULL)
	{
		fprintf(stderr, "Error allocating memory for references to each lines clues struct");
		return NULL;
	}

	/* Reading each line of clues */
	for (i = 0; i < (*width + *length); ++i)
	{
		j = 0;
		offset = 0;
		
		if (!fgets(fileLine, sizeof(fileLine), fPtr))
		{
			fprintf(stderr,"Error: EOF unexpectedly reached after line: %d\n", fileLineNum);
			return NULL;
		}
		++fileLineNum;

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
	
		if (j > 31)
		{
			fprintf(stderr,"Invalid contents on line %d: Too many clues (max 32).\n", fileLineNum);
			return NULL;
		}

		lineClues[i] = createLineClueStruct(clueBuffer, j);

		for (j = 0; j < lineClues[i]->clueCount; ++j)
		{
			printf("%d ", lineClues[i]->clues[j]);
		}

		printf("\n");
	}
	
	return lineClues;
}
