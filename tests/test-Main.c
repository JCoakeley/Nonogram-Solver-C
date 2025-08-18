#include <stdio.h>
#include "test-BitSet.h"
#include "test-GameBoard.h"
#include "test-Solver.h"

int main (void)
{
	int failures = 0, totalFailures = 0;

	failures += test_newBitSet();
	failures += test_setAllBits();
	if (failures == 0) printf("All BitSet tests passed!\n");

	totalFailures += failures;
	failures = test_createLine();

	if (failures == 0) printf("All Solver tests passed!\n");
	totalFailures += failures;


	if (totalFailures == 0) printf ("All tests passed!\n");

	else printf("%d tests failed.\n", totalFailures);

	return totalFailures;
}
