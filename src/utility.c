#include "../include/utility.h"
#include <stdlib.h>
#include <stdio.h>

/*
* TODO: Free malloc!!
* 
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

void printLineDetails (Line * line)
{
	int i;
	BitSet * bSet = line->bitSet;
	LineClue * lClue = line->clueSet;
	printf("Line %d:\n", line->lineId);
	printf("   Perm Count: %d\n", line->permutationCount);
	printf("   Store Count: %d\n", line->storeCount);
	printf("   Size: %d\n", line->size);
	printf("   MaskBits: %lX\n", line->maskBits);
	printf("   PartialBits: %lX\n", line->partialBits);
	printf("   Permutations:\n");

	for (i = 0; i < line->storeCount; ++i)
		printf("      %lX\n", line->permutations[i]);

	printf("   BitSet:\n");
	printf("      Word Count: %d\n", bSet->wordCount);
	printf("      Bit Count: %d\n", bSet->bitCount);
	printf("      Words:\n");

	for (i = 0; i < bSet->wordCount; ++i)
		printf("         %lX\n", bSet->words[i]);

	printf("   Clues:\n");
	printf("      Clue Count: %d\n", lClue->clueCount);
	printf("      Clues:\n");

	for (i = 0; i < lClue->clueCount; ++i)
		printf("         %d\n", lClue->clues[i]);

	return;
}

void printFormattedTime (long nano)
{
	double time = 0.0;
	
	if (nano > 1000000000)
	{	
		time = nano / 1000000000.0;
		printf("Time: %.3fs, ", time);
	}

	else if (nano > 1000000)
	{	
		time = nano / 1000000.0;
		printf("Time: %.3fms, ", time);
	}

	else if (nano > 1000)
	{	
		time = nano / 1000.0;
		printf("Time: %.3fμs, ", time);
	}

	else
		printf("Time: %ldns, ", nano);

	return;
}
