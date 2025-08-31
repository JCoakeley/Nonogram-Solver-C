#include "../include/utility.h"
#include <stdlib.h>
#include <stdio.h>
#include "../include/timing.h"

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

void printTimingData (Timings * timings)
{
	long totalNanos		= (timings->totalEnd.tv_sec - timings->totalStart.tv_sec) * 1000000000L 
						+ (timings->totalEnd.tv_nsec - timings->totalStart.tv_nsec);
	long fileNanos 		= (timings->fileEnd.tv_sec - timings->fileStart.tv_sec) * 1000000000L 
						+ (timings->fileEnd.tv_nsec - timings->fileStart.tv_nsec);
	long initNanos 		= (timings->initEnd.tv_sec - timings->initStart.tv_sec) * 1000000000L 
						+ (timings->initEnd.tv_nsec - timings->initStart.tv_nsec);
	long solvingNanos 	= (timings->solvingEnd.tv_sec - timings->solvingStart.tv_sec) * 1000000000L 
						+ (timings->solvingEnd.tv_nsec - timings->solvingStart.tv_nsec);
	long overlapNanos 	= (timings->overlapEnd.tv_sec - timings->overlapStart.tv_sec) * 1000000000L 
						+ (timings->overlapEnd.tv_nsec - timings->overlapStart.tv_nsec);
	long countingNanos 	= (timings->countEnd.tv_sec - timings->countStart.tv_sec) * 1000000000L 
						+ (timings->countEnd.tv_nsec - timings->countStart.tv_nsec);
	long generateNanos 	= (timings->generationEnd.tv_sec - timings->generationStart.tv_sec) * 1000000000L 
						+ (timings->generationEnd.tv_nsec - timings->generationStart.tv_nsec);
	long filteringNanos = (timings->filteringEnd.tv_sec - timings->filteringStart.tv_sec) * 1000000000L 
						+ (timings->filteringEnd.tv_nsec - timings->filteringStart.tv_nsec);
	long commonNanos 	= (timings->commonEnd.tv_sec - timings->commonStart.tv_sec) * 1000000000L 
						+ (timings->commonEnd.tv_nsec - timings->commonStart.tv_nsec);
	
	printf("Total Time: ");
	printFormattedTime(totalNanos);
	
	printf("\n\nFile Reading: ");
	printFormattedTime(fileNanos);
	printf("\nInitialization: ");
	printFormattedTime(initNanos);
	printf("\nOverlap: ");
	printFormattedTime(overlapNanos);
	printf("\nSolving Loop: ");
	printFormattedTime(solvingNanos);
	
	printf("\n\nCounting Permutations: ");
	printFormattedTime(countingNanos);
	printf("\nGenerating Permutations: ");
	printFormattedTime(generateNanos);
	printf("\nFiltering: ");
	printFormattedTime(filteringNanos);
	printf("\nGenerating Consistent Pattern: ");
	printFormattedTime(commonNanos);
	printf("\n");

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
 * Printed format includes three decimal places, e.g.:
 *   "4.231ms"
 *
 * Intended for performance timing output.
 */
void printFormattedTime (long nano)
{
	double time = 0.0;
	
	if (nano > 1000000000)
	{	
		time = nano / 1000000000.0;
		printf("%.3fs", time);
	}

	else if (nano > 1000000)
	{	
		time = nano / 1000000.0;
		printf("%.3fms", time);
	}

	else if (nano > 1000)
	{	
		time = nano / 1000.0;
		printf("%.3fμs", time);
	}

	else
		printf("%ldns", nano);

	return;
}
