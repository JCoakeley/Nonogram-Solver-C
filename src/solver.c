#include "../include/solver.h"
#include <stdlib.h>

void updateBitMasks (Line * line, const int * partialSolution)
{
	uint64_t partialBits = 0ULL, maskBits = 0ULL;
	int i, length = line->size;

	for (i = 0; i < length; ++i)
		if (partialSolution[i] != 0)
		{
			maskBits |= (1ULL << i);

			if (partialSolution[i] == 1)
				partialBits |= (1ULL << i);
		}

	line->partialBits = partialBits;
	line->maskBits = maskBits;
	
	return;
}
