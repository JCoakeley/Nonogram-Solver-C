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
	char totalPerms[64];
	char maxPerms[64];

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
	formatNumber(totalPerms, totalPermutations);
	formatNumber(maxPerms, maxPermutations);
	printf("\nIterations: %d\n", iterations);
	printf("Total Permutations Generated:  %s\n", totalPerms);
	printf("Maximum Possible Permutations: %s\n", maxPerms);

	return EXIT_SUCCESS;
}
