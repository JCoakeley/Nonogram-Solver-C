#include "test-SubLine.h"
#include <stdio.h>
#include "../include/solver.h"
#include "../include/utility.h"


int test_SubLine ()
{
	int returnValue = 0;

	int clues[] = {3, 3};
	LineClue clueSet = {clues, 2};
	Line * line = createLine(&clueSet, 10, 0);

	line->maskBits = 0x032;
	line->partialBits = 0x02;

	line->startEdge = createSubLine(&clueSet, 10);
	line->endEdge = createSubLine(&clueSet, 10);

	if (line->startEdge->size != 5 || line->endEdge->size != 5)
	{
		printf("Test Failuire: test_SubLine: StartEdge size: %d, EndEdge size: %d, Expected: 5\n",
			 line->startEdge->size, line->endEdge->size);
		++returnValue;
	}

	updateSubLineBitMasks(line);

	generateSubLinePermutationsStart(line->startEdge, 0, 0ULL, TRUE, 0, &(line->startEdge->permCount));
	generateSubLinePermutationsEnd(line->endEdge, line->clueSet->clueCount - 1, 0ULL, 1ULL << line->size, TRUE, 0, &(line->endEdge->permCount));

	if (line->startEdge->permCount != 2 || line->endEdge->permCount != 2)
	{
		printf("Test Failure: test_SubLine: StartEdge PermCount: %d - 2, EndEdge PermCount: %d - 2\n",
				line->startEdge->permCount, line->endEdge->permCount);
		++returnValue;
	}
	
	return returnValue;
}

int test_updateSubLineBitMasks ()
{
	int returnValue = 0;

	int clues1[] = {3, 3};
	LineClue clueSet1 = {clues1, 2};
	Line * line1 = createLine(&clueSet1, 10, 0);

	line1->maskBits = 0x231;
	line1->partialBits = 0x201;

	line1->startEdge = createSubLine(&clueSet1, 10);
	line1->endEdge = createSubLine(&clueSet1, 10);

	updateSubLineBitMasks(line1);

	if (line1->startEdge->maskBits != 0x11 || line1->startEdge->partialBits != 0x01)
	{
		printf("Test Failure: test_updateSubLineBitMasks: StartEdge MaskBits: %lX - 0x11, StartEdge PartialBits: %lX - 0x01",
				line1->startEdge->maskBits, line1->startEdge->partialBits);
		++returnValue;
	}

	if (line1->endEdge->maskBits != 0x220 || line1->endEdge->partialBits != 0x200)
	{
		printf("Test Failure: test_updateSubLineBitMasks: EndEdge MaskBits: %lX - 0x220, EndEdge PartialBits: %lX - 0x200",
				line1->endEdge->maskBits, line1->endEdge->partialBits);
		++returnValue;
	}

	int clues2[] = {1, 3, 3, 1};
	LineClue clueSet2 = {clues2, 4};
	Line * line2 = createLine(&clueSet2, 25, 1);

	line2->maskBits = 0x1807C03;
	line2->partialBits = 0x0806C01;

	line2->startEdge = createSubLine(&clueSet2, 25);
	line2->endEdge = createSubLine(&clueSet2, 25);

	updateSubLineBitMasks(line2);

	if (line2->startEdge->maskBits != 0xC03 || line2->startEdge->partialBits != 0xC01)
	{
		printf("Test Failure: test_updateSubLineBitMasks: StartEdge MaskBits: %lX - 0xC03, StartEdge PartialBits: %lX - 0xC01",
				line2->startEdge->maskBits, line2->startEdge->partialBits);
		++returnValue;
	}

	if (line2->endEdge->maskBits != 0x1806000 || line2->endEdge->partialBits != 0x0806000)
	{
		printf("Test Failure: test_updateSubLineBitMasks: EndEdge MaskBits: %lX - 0x1806000, EndEdge PartialBits: %lX - 0x0806000",
				line2->endEdge->maskBits, line2->endEdge->partialBits);
		++returnValue;
	}

	return returnValue;
}
