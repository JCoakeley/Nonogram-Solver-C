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
		++returnValue;
	}
	
	return returnValue;
}

int test_generatePermutations (void)
{
	int returnValue = 0;

	int clues[] = {7};
	LineClue lineclue = {(int *)&clues, 1};

	Line * line = createLine (&lineclue, 10, 0);

	generatePermutations(line, 0, 0ULL, 0, TRUE, &(line->permutationCount));

	if (line->permutationCount != 4)
	{
		printf("Test Failure: test_generatePermutations #1: Expected: 4, Actual: %d\n", line->permutationCount);
		++returnValue;
	}
	
	return returnValue;
}

int test_updateBitMask (void)
{
	int returnValue = 0;
	
	int partialSolution[] = {0, -1, 1, 1, -1};
	int clues[] = {3};
	uint64_t targetMaskBits = 0;
	uint64_t targetPartialBits = 0;
	LineClue lineclue = {(int *)&clues, 1};
	Line * line = createLine (&lineclue, 5, 0);

	updateBitMasks(line, (int *)&partialSolution);

	targetMaskBits |= 0x0D;
	targetPartialBits |= 0x0C;

	if (line->maskBits != targetMaskBits)
	{
		printf("Test Failure: test_updateBitMask #1: MaskBits no match\n");
		++returnValue;
	}

	if (line->partialBits != targetPartialBits)
	{
		printf("Test Failure: test_updateBitMask #1: PartialBits no match\n");
		++returnValue;
	}

	return returnValue;
}

int test_filterPermutations (void)
{
	int returnValue = 0;
		
	return returnValue;
}

int test_generateConsistentPattern (void);

int test_minRequiredLength (void)
{
	int returnValue = 0;
	int length = 0;

	int clues[] = {1, 1, 1};
	LineClue lineclue = {(int *)&clues, 3};

	Line * line = createLine (&lineclue, 5, 0);

	length = minRequiredLength(line);

	if (length != 5)
	{
		printf("Test Failure: test_minRequiredLength #1: Expected: 5, Actual: %d\n", length);
		++returnValue;
	}

	return returnValue;
}

int test_overlap (void)
{
	int returnValue = 0;
	uint64_t targetMaskBits = 0;
	uint64_t targetPartialBits = 0;
	int clues[] = {7};
	LineClue lineclue = {(int *)&clues, 1};

	Line * line = createLine (&lineclue, 10, 0);

	overlap(line);

	targetMaskBits |= 0x78;
	targetPartialBits |= 0x78;

	if (line->maskBits != targetMaskBits)
	{
		printf("Test Failure: test_overlap #1: MaskBits no match\n");
		++returnValue;
	}

	if (line->partialBits != targetPartialBits)
	{
		printf("Test Failure: test_overlap #1: PartialBits no match\n");
		++returnValue;
	}

	return returnValue;
}
