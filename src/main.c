#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include "../include/solverAPI.h"

int main (int argc, char ** argv)
{
	Timings timings = {0};
	timingStart(&timings, TOTAL);
	int iterations = 0, totalPermutations = 0;
	FILE * fPtr = NULL;
	int * gameBoard = NULL;

	timingStart(&timings, FILEREADING);
	fPtr = getFile(argc, argv[1]);

	gameBoard = solvePuzzle(fPtr, MODE_STANDARD, &iterations, &totalPermutations, &timings);

	free(gameBoard);
	gameBoard = NULL;
	
	fclose(fPtr);
	fPtr = NULL;

	timingEnd(&timings, TOTAL);
	printTimingData(&timings);
	printf("\nIterations: %d\n", iterations);
	printf("Total Permutations Generated: %d\n", totalPermutations);

	return EXIT_SUCCESS;
}
