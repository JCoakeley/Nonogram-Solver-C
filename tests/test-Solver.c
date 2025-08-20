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
	/* 4 choose 2 */

	int clues2[] = {1, 3, 5};
	LineClue lineclue2 = {(int *)&clues2, 3};
	Line * line2 = createLine (&lineclue2, 20, 1);
	/* 12 choose 3 */

	int clues3[] = {6, 3, 5, 1, 7};
	LineClue lineclue3 = {(int *)&clues3, 5};
	Line * line3 = createLine (&lineclue3, 40, 2);
	/* 19 choose 5 */

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

	int clues1[] = {1, 1, 1};
	LineClue lineclue1 = {(int *)&clues1, 3};
	Line * line1 = createLine (&lineclue1, 5, 0);
	/* Length = 5 */

	int clues2[] = {1, 3, 8, 5};
	LineClue lineclue2 = {(int *)&clues2, 4};
	Line * line2 = createLine (&lineclue2, 25, 0);
	/* Length = 20 */

	int clues3[] = {7, 1, 8, 2, 1, 5, 7, 3, 3};
	LineClue lineclue3 = {(int *)&clues3, 9};
	Line * line3 = createLine (&lineclue3, 50, 0);
	/* Length = 45 */

	length = minRequiredLength(line1);

	if (length != 5)
	{
		printf("Test Failure: test_minRequiredLength #1: Expected: 5, Actual: %d\n", length);
		++returnValue;
	}

	length = minRequiredLength(line2);
	
	if (length != 20)
	{
		printf("Test Failure: test_minRequiredLength #2: Expected: 20, Actual: %d\n", length);
		++returnValue;
	}

	length = minRequiredLength(line3);
	
	if (length != 45)
	{
		printf("Test Failure: test_minRequiredLength #3: Expected: 45, Actual: %d\n", length);
		++returnValue;
	}

	return returnValue;
}

int test_overlap (void)
{
	int returnValue = 0;
	uint64_t targetMaskBits = 0;
	uint64_t targetPartialBits = 0;
	
	int clues1[] = {7};
	LineClue lineclue1 = {(int *)&clues1, 1};
	Line * line1 = createLine (&lineclue1, 10, 0);
	/* 00011 11000 */

	int clues2[] = {14, 6};
	LineClue lineclue2 = {(int *)&clues2, 2};
	Line * line2 = createLine (&lineclue2, 30, 0);
	/* 00000 00001 11110 00000 00000 00000 */

	int clues3[] = {15, 2, 8, 2, 13};
	LineClue lineclue3 = {(int *)&clues3, 5};
	Line * line3 = createLine (&lineclue3, 50, 0);
	/* 00000 01111 11111 00000 00000 11000 00000 00111 11110 00000 */

	overlap(line1);

	targetMaskBits = 0x78;
	targetPartialBits = 0x78;

	if (line1->maskBits != targetMaskBits)
	{
		printf("Test Failure: test_overlap #1: MaskBits no match\n");
		++returnValue;
	}

	if (line1->partialBits != targetPartialBits)
	{
		printf("Test Failure: test_overlap #1: PartialBits no match\n");
		++returnValue;
	}

	overlap(line2);
	
	targetMaskBits = 0x3E00;
	targetPartialBits = 0x3E00;

	if (line2->maskBits != targetMaskBits)
	{
		printf("Test Failure: test_overlap #2: MaskBits no match\n");
		++returnValue;
	}

	if (line2->partialBits != targetPartialBits)
	{
		printf("Test Failure: test_overlap #2: PartialBits no match\n");
		++returnValue;
	}

	overlap(line3);
		
	targetMaskBits = 0xFE006007FC0;
	targetPartialBits = 0xFE006007FC0;

	if (line3->maskBits != targetMaskBits)
	{
		printf("Test Failure: test_overlap #3: MaskBits no match\n");
		++returnValue;
	}

	if (line3->partialBits != targetPartialBits)
	{
		printf("Test Failure: test_overlap #3: PartialBits no match\n");
		++returnValue;
	}

	return returnValue;
}
