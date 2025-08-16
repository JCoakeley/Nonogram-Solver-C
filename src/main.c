#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/main.h"

int main (int argc, char ** argv)
{
	char readChar;
	int gameBoardWidth = 0, gameBoardLength = 0;
	FILE * fPtr = NULL;
	LineClues * lineClues = NULL;

	fPtr = getFile(argc, argv[1]);

	lineClues = readFile(fPtr, &gameBoardWidth, &gameBoardLength);

	printf("Width: %d\nLength: %d\n", gameBoardWidth, gameBoardLength);

	while ((readChar = fgetc(fPtr)) != EOF)
		printf("%c", readChar);

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
				printf("Error reading input\n");
				loop = TRUE;
				continue;
			}

			fileName[strcspn(fileName, "\n")] = '\0';

		    if (fileName[0] == '\0') 
		    {
		        printf("Filename cannot be empty.\n");
		        loop = TRUE;
		        continue;
		    }
		}

		else strcpy(fileName, argv);

		argc = 1;
		fPtr = fopen(fileName, "r");

		if (fPtr == NULL)
		{
			printf("Failed to open file: %s\n", fileName);
			loop = TRUE;
		}
	} while (loop);
	
	return fPtr;
}

LineClues * readFile (FILE * fPtr, int * width, int *length)
{
	char fileLine[256], * rest = NULL;
	int numCharsRead = 0, fileLineNum = 0, matchCount = 0;

	/* Reading gameboard width and length from file with validation checking */
	if (!fgets(fileLine, sizeof(fileLine), fPtr))
	{
		printf("Error: Empty file or read error.\n");
		return NULL;
	}
	++fileLineNum;
	
	matchCount = sscanf(fileLine, "%d %d %n", width, length, &numCharsRead);
	if (matchCount != 2)
	{
		printf("Invalid file contents line: %d, Expecting a width and length.\n", fileLineNum);
		return NULL;
	}

	rest = fileLine + numCharsRead;

	while (*rest != '\0' && isspace((unsigned char) *rest)) ++rest;

	if (*rest != '\0')
	{
		printf("Invalid file contents line: %d, Expecting a width and length.\n", fileLineNum);
		return NULL;
	}

	/* Reading each line of clues TODO */
	while (fgets(fileLine, sizeof(fileLine), fPtr))
	{
		
	}
	
	return NULL; /* Placeholder */
}
