#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include <string.h>
#include "../include/solverAPI.h"

int main (int argc, char ** argv)
{
	Timings timings = {0};
	timingStart(&timings, TOTAL);
	int i, iterations = 0, totalPermutations = 0, maxPermutations = 0;
	FILE * fPtr = NULL;
	int * gameBoard = NULL;
	char * fileName = NULL;
	SolvingMode mode = MODE_STANDARD;

	for (i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--benchmark") == 0)
			mode = MODE_BENCHMARK;
			
		else
			fileName = argv[i];  // first non-flag is treated as filename
	}

	timingStart(&timings, FILEREADING);
	fPtr = getFile(fileName);

	gameBoard = solvePuzzle(fPtr, mode, &iterations, &totalPermutations, &maxPermutations, &timings);

	free(gameBoard);
	gameBoard = NULL;
	
	fclose(fPtr);
	fPtr = NULL;

	timingEnd(&timings, TOTAL);
	printTimingData(&timings);
	printf("\nIterations: %d\n", iterations);
	printf("Total Permutations Generated: %d\n", totalPermutations);
	printf("Maximun Possible Permutations: %d\n", maxPermutations);

	return EXIT_SUCCESS;
}
