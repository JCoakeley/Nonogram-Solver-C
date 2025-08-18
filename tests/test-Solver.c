#include "test-Solver.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/solver.h"
#include "../include/utility.h"

int test_createLine (void)
{
	int returnValue = 0;

	int clues[] = {1, 1, 1};
	LineClue lineclue = {(int *)&clues, 3};

	Line * line = createLine (&lineclue, 5, 0);

	if (line->size != 5 || line->lineId != 0 || line->clues->clueCount != 3)
	{
		printf("Test Failure: test_createLine #1\n");
		returnValue = 1;
	}
	
	return returnValue;
}

int test_updateBitMask (void);

int test_filterPermutations (void);

int test_generateConsistentPattern (void);

int test_minRequiredLength (void);

int test_overlap (void);
