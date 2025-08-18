#include "../include/utility.h"
#include <stdlib.h>

/*
* Takes in an array of integers that is a clueBuffer and a count of the number
* of integers in the buffer. Allocates the memory needed for a LineClue struct
* and for the integer array referenced in the struct then adds all the integers
* from the buffer up to the specified amount and returns a pointer to the
* created LineClue struct. Will return NULL if either memory allocation step
* fail, it is the resposibility of the calling function to act on this error.
*/
LineClue * createLineClueSet(int * clueBuffer, int clueCount)
{
	int i;
	LineClue * lineClueSet = (LineClue *)malloc(sizeof(LineClue));

	if (lineClueSet == NULL) return NULL;

	/* Initializing struct data elements */
	lineClueSet->clueCount = clueCount;
	lineClueSet->clues = (int *)malloc(sizeof(int) * clueCount);

	if (lineClueSet->clues == NULL)
	{
		free(lineClueSet);
		return NULL;
	}

	/* Copying integers from the buffer up to the specified count */
	for (i = 0; i < clueCount; ++i)
		lineClueSet->clues[i] = clueBuffer[i];
	
	return lineClueSet;
}
