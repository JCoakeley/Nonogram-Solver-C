#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/fileIO.h"
#include "../include/solverAPI.h"

FILE * getFile (int, char *);

LineClue ** readFile (FILE *, int *, int *);

int main (int argc, char ** argv)
{
	struct timespec endTime, startTime;
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	int iterations = 0;
	FILE * fPtr = NULL;
	int * gameBoard = NULL;

	fPtr = getFile(argc, argv[1]);

	gameBoard = solvePuzzle(fPtr, 0, &iterations);

	free(gameBoard);
	gameBoard = NULL;
	
	fclose(fPtr);
	fPtr = NULL;

	clock_gettime(CLOCK_MONOTONIC, &endTime);

	long nanos = (endTime.tv_sec - startTime.tv_sec) * 1000000000L + (endTime.tv_nsec - startTime.tv_nsec);

	printFormattedTime(nanos);
	printf("Iterations: %d\n", iterations);

	return EXIT_SUCCESS;
}
