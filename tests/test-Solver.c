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

	if (line->size != 5 || line->lineId != 0 || line->clueSet->clueCount != 3)
	{
		printf("Test Failure: test_createLine #1\n");
		++returnValue;
	}
	
	return returnValue;
}

int test_generatePermutations_counting (void)
{
	int returnValue = 0;

	int clues1[] = {4, 3};
	LineClue lineclue1 = {(int *)&clues1, 2};
	Line * line1 = createLine (&lineclue1, 10, 0);

	int clues2[] = {1, 3, 5};
	LineClue lineclue2 = {(int *)&clues2, 3};
	Line * line2 = createLine (&lineclue2, 20, 1); /* 220 */

	int clues3[] = {6, 3, 5, 1, 7};
	LineClue lineclue3 = {(int *)&clues3, 5};
	Line * line3 = createLine (&lineclue3, 40, 2); /* 11628 */	

	generatePermutations(line1, 0, 0ULL, 0, TRUE, &(line1->permutationCount));

	if (line1->permutationCount != 6)
	{
		printf("Test Failure: test_generatePermutations #1: Expected: 6, Actual: %d\n", line1->permutationCount);
		++returnValue;
	}

	generatePermutations(line2, 0, 0ULL, 0, TRUE, &(line2->permutationCount));
	
	if (line2->permutationCount != 220)
	{
		printf("Test Failure: test_generatePermutations #2: Expected: 220, Actual: %d\n", line2->permutationCount);
		++returnValue;
	}

	generatePermutations(line3, 0, 0ULL, 0, TRUE, &(line3->permutationCount));
		
	if (line3->permutationCount != 11628)
	{
		printf("Test Failure: test_generatePermutations #3: Expected: 11628, Actual: %d\n", line3->permutationCount);
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
