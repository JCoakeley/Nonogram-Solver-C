#include <stdio.h>
#include "test-BitSet.h"
#include "test-GameBoard.h"
#include "test-Solver.h"
#include "test-FullPuzzles.h"
#include "test-SubLine.h"

int main (void)
{
	int failures = 0, totalFailures = 0;

	failures += test_newBitSet();
	failures += test_setAllBits();
	failures += test_clearBit();
	failures += test_nextSetBit();

	if (failures == 0) printf("All BitSet tests passed!\n");
	totalFailures += failures;
	
	failures = test_createLine();
	failures += test_updateBitMask();
	failures += test_minRequiredLength();
	failures += test_overlap();
	failures += test_generatePermutations_counting();

	if (failures == 0) printf("All Solver tests passed!\n");
	totalFailures += failures;

	failures = test_SubLine();
	failures +=test_updateSubLineBitMasks();
	
	if (failures == 0) printf("All SubLine tests passed!\n");
	totalFailures += failures;

	failures = test_fullPuzzles();

	if (failures == 0) printf("All Full Puzzle tests passed!\n");
	totalFailures += failures;

	if (totalFailures == 0) printf ("\nAll Tests Passed!\n");

	else printf("\n%d Tests Failed.\n", totalFailures);

	return totalFailures;
}
