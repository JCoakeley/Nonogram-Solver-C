#include "../include/utility.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Allocates and initializes a LineClue struct from the provided buffer of clue integers.
 *
 * Parameters:
 * - clueBuffer : Pointer to an array of integers representing the clue sequence.
 * - clueCount  : The number of integers to copy from the clueBuffer.
 *
 * Workflow:
 * - Allocates memory for the LineClue struct.
 * - Allocates memory for the internal clue array of size `clueCount`.
 * - Copies all integers from clueBuffer into the newly allocated array.
 *
 * Return:
 * - A pointer to a fully initialized LineClue struct on success.
 * - NULL if either memory allocation fails. (Responsibility of caller to handle this.)
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

/*
 * Prints detailed debugging information about a single Line.
 *
 * Output includes:
 * - Line ID, permutation and storage counts, line size
 * - maskBits and partialBits as hex values
 * - All stored permutations (hex)
 * - Associated BitSet structure (word/bit counts and words)
 * - ClueSet details (number of clues and individual clue values)
 *
 * Intended for debugging and inspecting the internal state of a Line after solving steps.
 */
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

/*
 * Prints a formatted version of a time duration in nanoseconds.
 *
 * Depending on the size of the input, it prints:
 * - seconds if > 1 second
 * - milliseconds if > 1 millisecond
 * - microseconds if > 1 microsecond
 * - nanoseconds otherwise
 *
 * Printed format includes three decimal places and a trailing comma, e.g.:
 *   "Time: 4.231ms, "
 *
 * Intended for performance timing output.
 */
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
