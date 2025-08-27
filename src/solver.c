#include "../include/solver.h"
#include "../include/utility.h"
#include <stdlib.h>
#include <stdio.h>

/* 
* Creates a new line and populates each of it data elements. Size, clues and
* lineId are all passed in as arguments. PermCount, maskBits and partialBits
* always start out at 0. Permutations will be added at generation time,
* permCount will be updated then and a bitSet will be created.
*/
Line * createLine (LineClue * clues, int size, int lineId)
{
	Line * line = (Line *)malloc(sizeof(Line));

	if (line == NULL) return line;

	line->size 				= size;
	line->lineId 			= lineId;
	line->clueSet 			= clues;
	line->permutationCount 	= 0;
	line->storeCount		= 0;
	line->maskBits 			= 0ULL;
	line->partialBits 		= 0ULL;
	line->bitSet 			= NULL;
	line->permutations 		= NULL;

	return line;
}

void generatePermutations (Line * line, int clueIndex, uint64_t current, int position, bool countOnly, int * permCount)
{
	int groupSize, maxStart, newPosition, start;
	uint64_t groupBits, newBits, writtenBitsMask, compareMask;

	if (clueIndex >= line->clueSet->clueCount)
	{
		if (((current & line->maskBits) ^ line->partialBits) == 0)
		{
			if (countOnly)
				(*permCount)++;

			else
				line->permutations[(*permCount)++] = current;
		}

		return;
	}
	
	groupSize = line->clueSet->clues[clueIndex];
	maxStart = line->size - totalRemainingLength(line, clueIndex);

	for (start = position; start <= maxStart; ++start)
	{
		groupBits = ((1ULL <<groupSize) - 1ULL) << start;
		newBits = current | groupBits;

		newPosition = start + groupSize + 1;

		writtenBitsMask = (1ULL << newPosition) - 1ULL;
		compareMask = writtenBitsMask & line->maskBits;

		if (((newBits & compareMask) ^ (line->partialBits)) != 0)
			continue;

		generatePermutations(line, clueIndex + 1, newBits, newPosition, countOnly, permCount);
	}
}

int totalRemainingLength (Line * line, int clueIndex)
{
	int i, length = 0, size = line->clueSet->clueCount;

	for (i = clueIndex; i < size; ++i)
		length += line->clueSet->clues[i];

	length += (size - clueIndex - 1);

	return length;
}

/*
* TODO: May change to directly update from the gameBoard instead of making
* the partialSolution array as an intermediate step.
* 
* Updates the maskBits and partialBits to match the current partial solution.
* maskBits on set for all solved cells, partialBits are set for all filled
* solved cells.
*/
void updateBitMasks (Line * line, const int * partialSolution)
{
	uint64_t partialBits = 0ULL, maskBits = 0ULL;
	int i, length = line->size;

	for (i = 0; i < length; ++i)
		if (partialSolution[i] != -1)
		{
			maskBits |= (1ULL << i);

			if (partialSolution[i] == 1)
				partialBits |= (1ULL << i);
		}

	line->partialBits = partialBits;
	line->maskBits = maskBits;
	
	return;
}

/*
* Filters out permutations that don't fit with the current mask and partial
* bits. The maskBits set which bits of the permutations must match those
* of the partialBits. If a permutation does not match then the corresponding
* bitSet bit is cleared.
*/
void filterPermutations (Line * line)
{
	int i;
	uint64_t * const perms 	= line->permutations;
	const uint64_t 	mask 	= line->maskBits;
	const uint64_t 	partial = line->partialBits;
	BitSet * const 	bSet 	= line->bitSet;

	if (mask != 0)
	{
		for (i = nextSetBit(bSet, 0); i >= 0; i = nextSetBit(bSet, i + 1))
			if (((perms[i] & mask) ^ partial) != 0)
				clearBit(bSet, i);
	}
					
	return;
}

/*
* Iterates through all valid permutations tracking which bits are always 0 and
* aways 1 across all permutations. Breaks early if there are no bits that are
* consistent across all permutations. Sets the mask and partial bits based the
* solved1s and solved0s bits. Mask and partial are used to update the gameBoard.
*/
void generateConsistentPattern (Line * line)
{
	int i;
	BitSet * const bSet		= line->bitSet;
	uint64_t * const perms 	= line->permutations;
	uint64_t widthMask 		= (1ULL << line->size) - 1ULL;
	uint64_t andMask 		= widthMask;
	uint64_t orMask 		= 0ULL;
	uint64_t unsolved 		= ~(line->maskBits) & widthMask;
	uint64_t solved1s, solved0s;

	for (i = nextSetBit(bSet, 0); i >= 0; i = nextSetBit(bSet, i + 1))
	{
		andMask &= perms[i];
		orMask |= perms[i];

		if (((andMask & unsolved) | (~orMask & unsolved)) == 0)
			break;
	}

	solved1s = andMask & unsolved;
	solved0s = ~orMask & unsolved;

	line->maskBits |= (solved1s | solved0s);
	line->partialBits |= solved1s;
	
	return;
}

/* 
* Computes the minimum size require to fit a line's clues one after
* the other with the minimum 1 space between each clue. This length
* is returned.
*/
int minRequiredLength (Line * line)
{
	int i, total = 0;

	for (i = 0; i < line->clueSet->clueCount; ++i)
		total += line->clueSet->clues[i];

	total += line->clueSet->clueCount - 1;

	return total;
}

/*
* Update the mask and partial bits of the supplied line to reflect
* known solved filled cells based on overlap between the clues between
* left justified and right justified. The mask and partial bits are
* used to update the gameboard.
*/
void overlap (Line * line)
{
	int i, j, leftEnd, rightStart;
	int leftPos = 0;
	int rightPos = line->size - minRequiredLength(line);
	int * clues = line->clueSet->clues;

	for (i = 0; i < line->clueSet->clueCount; ++i)
	{
		leftEnd = leftPos + clues[i];
		leftPos = leftEnd + 1;

		rightStart = rightPos;
		rightPos += clues[i] + 1;

		/* j is 0 indexed. Will set a 1 in the mask and partial bit should
			any clues rightmost starting position be less than it's leftmost
			ending position, leftEnd is 1 past the last filled cell. */
		for (j = rightStart; j < leftEnd; ++j)
		{
			line->maskBits |= 1ULL << j;
			line->partialBits |= 1ULL << j;
		}
	}
	return;
}
