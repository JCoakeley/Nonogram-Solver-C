#include "../include/solver.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Creates and initializes a new Line struct.
 *
 * Parameters:
 * - clues: Pointer to a LineClue struct for the line
 * - size: Number of cells in the line
 * - lineId: The line’s unique identifier (row or column index)
 *
 * Returns a pointer to the initialized Line. The following fields are set:
 * - maskBits, partialBits, permutationCount, storeCount → all start at 0
 * - bitSet and permutations → NULL (to be set during generation)
 *
 * The caller is responsible for allocating permutations and bitSet later.
 */
Line * createLine (LineClue * clues, int size, int lineId)
{
	Line * line = (Line *)malloc(sizeof(Line));
	int n = 0;

	if (line == NULL) return line;

	n = size + clues->clueCount - minRequiredLength(clues);

	line->size 				= size;
	line->lineId 			= lineId;
	line->clueSet 			= clues;
	line->storeCount		= 0;
	line->maskBits 			= 0ULL;
	line->partialBits 		= 0ULL;
	line->maxPermutations	= nCr(n, clues->clueCount);
	line->bitSet 			= NULL;
	line->setBitIndexes		= NULL;
	line->permutations 		= NULL;
	line->startEdge			= NULL;
	line->endEdge			= NULL;
	line->state				= LINE_ALLOC_NONE;
	line->genDirection		= DIRECTION_NONE;

	return line;
}

SubLine * createSubLine (LineClue * clues, int lineSize)
{
	SubLine * subLine = (SubLine *)malloc(sizeof(SubLine));

	if (subLine == NULL) return subLine;
	
	subLine->size 			= (lineSize / 2) < 15 ? (lineSize / 2) : 15;
	subLine->lineSize		= lineSize;
	subLine->permCount		= 0;
	subLine->storeCount 	= 0;
	subLine->clueSet		= clues;
	subLine->maskBits 		= 0ULL;
	subLine->partialBits 	= 0ULL;
	subLine->bitSet 		= NULL;
	subLine->permutations 	= NULL;
	subLine->state 			= LINE_ALLOC_NONE;
	
	return subLine;
}

void generatePermutations (Line * line, int * permCount)
{
	if (line->genDirection == DIRECTION_NONE)
		generationDirection(line);

	if (line->genDirection == DIRECTION_START)
		generatePermutationsStart(line, 0, 0ULL, 0, permCount);

	else
		generatePermutationsEnd(line, line->clueSet->clueCount - 1, 0ULL, 1ULL << line->size, 0, permCount);
}

/*
 * Recursively generates all valid permutations of a line that are consistent with
 * the current maskBits and partialBits. Generates permutations from the first clue
 * to the last clue ie Direction Start.
 *
 * Parameters:
 * - line: The Line being solved
 * - clueIndex: Index of the clue currently being placed
 * - current: Current state of the permutation (bit pattern)
 * - position: Starting position for placing the next clue
 * - countOnly: If true, only counts permutations without storing them
 * - permCount: Pointer to either count or index for storing
 *
 * Early-prunes branches that conflict with known solved cells using bitmask checks.
 */
void generatePermutationsStart (Line * line, int clueIndex, uint64_t current, int position, int * permCount)
{
	int groupSize, maxStart, newPosition, start;
	uint64_t groupBits, newBits, writtenBitsMask, compareMask;

	/* Base Case: All clues placed in the permutation */
	if (clueIndex >= line->clueSet->clueCount)
	{
		line->permutations[(*permCount)++] = current;

		return;
	}
	
	groupSize = line->clueSet->clues[clueIndex];
	maxStart = line->size - totalRemainingLengthStart(line, clueIndex);

	/* Looping through each possible valid place for a specific clue within the permutation. */
	for (start = position; start <= maxStart; ++start)
	{
		groupBits = ((1ULL << groupSize) - 1ULL) << start;
		newBits = current | groupBits;

		newPosition = start + groupSize + 1;

		writtenBitsMask = (1ULL << newPosition) - 1ULL;

		/* Setting the bits within the range of the current partial permutation that are also solved
			on the gameboard. */
		compareMask = writtenBitsMask & line->maskBits;

		/* Early pruning of branches that don't fit the mask and partial bits */
		if ( ( (newBits & compareMask) ^ (line->partialBits & compareMask) ) != 0 )
			continue;

		generatePermutationsStart(line, clueIndex + 1, newBits, newPosition, permCount);
	}
}

void generateSubLinePermutationsStart (SubLine * subLine, int clueIndex, uint64_t current, char countOnly, int position, int * permCount)
{
	int groupSize, maxStart, newPosition, start;
	uint64_t groupBits, newBits, writtenBitsMask, compareMask;

	/* Base Case: All clues placed in the permutation */
	if (clueIndex >= subLine->clueSet->clueCount || position >= subLine->size)
	{
		/* Only counting or storing permutations that fit the mask and partial bits */
		if (((current & subLine->maskBits) ^ subLine->partialBits) == 0)
		{
			if (countOnly)
				++(*permCount);

			else
				subLine->permutations[(*permCount)++] = current;
		}

		return;
	}
	
	groupSize = subLine->clueSet->clues[clueIndex];
	maxStart = subLine->lineSize - totalRemainingLengthStartSubLine(subLine, clueIndex);

	/* Looping through each possible valid place for a specific clue within the permutation. */
	for (start = position; start <= maxStart; ++start)
	{	
		groupBits = ((1ULL << groupSize) - 1ULL) << start;
		newBits = current | groupBits;

		newPosition = start + groupSize + 1;

		writtenBitsMask = (1ULL << newPosition) - 1ULL;

		/* Setting the bits within the range of the current partial permutation that are also solved
			on the gameboard. */
		compareMask = writtenBitsMask & subLine->maskBits;

		/* Early pruning of branches that don't fit the mask and partial bits */
		if ( ( (newBits & compareMask) ^ (subLine->partialBits & compareMask) ) != 0 )
			continue;

		generateSubLinePermutationsStart(subLine, clueIndex + 1, newBits, countOnly, newPosition, permCount);

		if (start >= subLine->size) break;
	}
}

/*
 * Computes the minimum number of cells required to place all clues starting
 * from the given clueIndex, including the required spaces between clues.
 *
 * Used to limit clue placement during permutation generation.
 */
int totalRemainingLengthStart (Line * line, int clueIndex)
{
	int i, length = 0, size = line->clueSet->clueCount;

	for (i = clueIndex; i < size; ++i)
		length += line->clueSet->clues[i];

	length += (size - clueIndex - 1);

	return length;
}

int totalRemainingLengthStartSubLine (SubLine * subLine, int clueIndex)
{
	int i, length = 0, size = subLine->clueSet->clueCount;

	for (i = clueIndex; i < size; ++i)
		length += subLine->clueSet->clues[i];

	length += (size - clueIndex - 1);

	return length;
}

/*
 * Recursively generates all valid permutations of a line that are consistent with
 * the current maskBits and partialBits. Generates permutations from the first clue
 * to the last clue ie Direction Start.
 *
 * Parameters:
 * - line: The Line being solved
 * - clueIndex: Index of the clue currently being placed
 * - current: Current state of the permutation (bit pattern)
 * - position: Starting position for placing the next clue
 * - countOnly: If true, only counts permutations without storing them
 * - permCount: Pointer to either count or index for storing
 *
 * Early-prunes branches that conflict with known solved cells using bitmask checks.
 */
void generatePermutationsEnd (Line * line, int clueIndex, uint64_t current, uint64_t sizeBits, int position, int * permCount)
{
	int groupSize, maxStart, newPosition, start;
	uint64_t groupBits, newBits, writtenBitsMask, compareMask;

	/* Base Case: All clues placed in the permutation */
	if (clueIndex < 0)
	{
		line->permutations[(*permCount)++] = current;

		return;
	}
	
	groupSize = line->clueSet->clues[clueIndex];
	maxStart = line->size - totalRemainingLengthEnd(line, clueIndex);

	/* Looping through each possible valid place for a specific clue within the permutation. */
	for (start = position; start <= maxStart; ++start)
	{
		groupBits = (sizeBits - (sizeBits >> groupSize)) >> start;
		newBits = current | groupBits;

		newPosition = start + groupSize + 1;

		writtenBitsMask = sizeBits - (sizeBits >> newPosition);

		/* Setting the bits within the range of the current partial permutation that are also solved
			on the gameboard. */
		compareMask = writtenBitsMask & line->maskBits;

		/* Early pruning of branches that don't fit the mask and partial bits */
		if ( ( (newBits & compareMask) ^ (line->partialBits & compareMask) ) != 0 )
			continue;

		generatePermutationsEnd(line, clueIndex - 1, newBits, sizeBits, newPosition, permCount);
	}
}

void generateSubLinePermutationsEnd (SubLine * subLine, int clueIndex, uint64_t current, uint64_t sizeBits, char countOnly, int position, int * permCount)
{
	int groupSize, maxStart, newPosition, start;
	uint64_t groupBits, newBits, writtenBitsMask, compareMask;

	/* Base Case: All clues placed in the permutation */
	if (clueIndex < 0 || position >= subLine->size)
	{
		/* Only counting or storing permutations that fit the mask and partial bits */
		if (((current & subLine->maskBits) ^ subLine->partialBits) == 0)
		{
			if (countOnly)
				++(*permCount);

			else
				subLine->permutations[(*permCount)++] = current;
		}

		return;
	}
	
	groupSize = subLine->clueSet->clues[clueIndex];
	maxStart = subLine->lineSize - totalRemainingLengthEndSubLine(subLine, clueIndex);

	/* Looping through each possible valid place for a specific clue within the permutation. */
	for (start = position; start <= maxStart; ++start)
	{
		groupBits = (sizeBits - (sizeBits >> groupSize)) >> start;
		newBits = current | groupBits;

		newPosition = start + groupSize + 1;

		writtenBitsMask = sizeBits - (sizeBits >> newPosition);

		/* Setting the bits within the range of the current partial permutation that are also solved
			on the gameboard. */
		compareMask = writtenBitsMask & subLine->maskBits;

		/* Early pruning of branches that don't fit the mask and partial bits */
		if ( ( (newBits & compareMask) ^ (subLine->partialBits & compareMask) ) != 0 )
			continue;

		generateSubLinePermutationsEnd(subLine, clueIndex - 1, newBits, sizeBits, countOnly, newPosition, permCount);

		if (start >= subLine->size) break;
	}
}

/*
 * Computes the minimum number of cells required to place all clues ending
 * from the given clueIndex, including the required spaces between clues.
 *
 * Used to limit clue placement during permutation generation.
 */
int totalRemainingLengthEnd (Line * line, int clueIndex)
{
	int i, length = 0;

	for (i = clueIndex; i >= 0; --i)
		length += line->clueSet->clues[i];

	length += clueIndex;

	return length;
}

int totalRemainingLengthEndSubLine (SubLine * subLine, int clueIndex)
{
	int i, length = 0;

	for (i = clueIndex; i >= 0; --i)
		length += subLine->clueSet->clues[i];

	length += clueIndex;

	return length;
}

/*
 * Updates the Line’s maskBits and partialBits based on a given partial solution array.
 *
 * For each solved cell:
 * - maskBits is set to 1
 * - partialBits is set to 1 only for filled cells (value == 1)
 *
 * TODO: Consider updating mask/partial bits directly from gameBoard instead
 * of using an intermediate array.
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

void updateSubLineBitMasks (Line * line)
{
	uint64_t sizeBits = 1ULL << line->size;
	uint64_t startMask = (1ULL << line->startEdge->size) - 1ULL;
	uint64_t endMask = sizeBits - (sizeBits >> line->endEdge->size);

	line->startEdge->partialBits = line->partialBits & startMask;
	line->startEdge->maskBits 	 = line->maskBits & startMask;
	line->endEdge->partialBits 	 = line->partialBits & endMask;
	line->endEdge->maskBits 	 = line->maskBits & endMask;
	return;
}

void updateBitMasksFromSubLines (Line * line)
{
	line->maskBits 	  |= line->startEdge->maskBits;
	line->partialBits |= line->startEdge->partialBits;
	line->maskBits 	  |= line->endEdge->maskBits;
	line->partialBits |= line->endEdge->partialBits;

	return;
}

/*
 * Filters out any stored permutations that are inconsistent with the current
 * maskBits and partialBits.
 *
 * For each valid (set) permutation:
 * - If its bits don’t match the solved state (mask & partial), the corresponding
 *   bit is cleared in the BitSet.
 */
void filterPermutations (Line * line)
{
	int i;
	uint64_t * const perms 	= line->permutations;
	const uint64_t 	mask 	= line->maskBits;
	const uint64_t 	partial = line->partialBits;
	BitSet * const bSet		= line->bitSet;
	const int setBitCount 	= allSetBits(bSet, line->setBitIndexes);
	const int * setBitIndexes = line->setBitIndexes;

	if (mask != 0)
	{
		for (i = 0; i < setBitCount; ++i)
			if ((perms[setBitIndexes[i]] & mask) != partial)
				clearBit(bSet, setBitIndexes[i]);
	}
					
	return;
}

void filterSubLinePermutations (Line * line)
{
	int i;
	uint64_t * perms   = line->startEdge->permutations;
	uint64_t   mask    = line->startEdge->maskBits;
	uint64_t   partial = line->startEdge->partialBits;
	BitSet *   bSet    = line->startEdge->bitSet;

	if (mask != 0)
	{
		for (i = nextSetBit(bSet, 0); i >= 0; i = nextSetBit(bSet, i + 1))
			if ((perms[i] & mask) != partial)
				clearBit(bSet, i);
	}

	perms 	= line->endEdge->permutations;
	mask 	= line->endEdge->maskBits;
	partial = line->endEdge->partialBits;
	bSet 	= line->endEdge->bitSet;

	if (mask != 0)
	{
		for (i = nextSetBit(bSet, 0); i >= 0; i = nextSetBit(bSet, i + 1))
			if ((perms[i] & mask) != partial)
				clearBit(bSet, i);
	}
}

/*
 * Identifies bits that are consistent across all remaining valid permutations.
 *
 * For any bit position:
 * - If all permutations agree on 1 → mark it as a solved filled cell
 * - If all permutations agree on 0 → mark it as a solved empty cell
 *
 * Updates the Line’s maskBits and partialBits with this information.
 */
void generateConsistentPattern (Line * line)
{
	int i;
	BitSet * const bSet		= line->bitSet;
	const int setBitCount 	= allSetBits(bSet, line->setBitIndexes);
	const int * setBitIndexes = line->setBitIndexes;
	uint64_t * const perms 	= line->permutations;
	uint64_t widthMask 		= (1ULL << line->size) - 1ULL;
	uint64_t andMask 		= widthMask;
	uint64_t orMask 		= 0ULL;
	uint64_t unsolved 		= ~(line->maskBits) & widthMask;
	uint64_t solved1s, solved0s;

	/* Looping through all valid permutations using the andMask to track which bits are always 1s
		in every permutation and the orMask for 0s. Breaks early if there are no bits that are always
		1 or 0 through all permutations */
	for (i = 0; i < setBitCount; ++i)
	{
		andMask &= perms[setBitIndexes[i]];
		orMask |= perms[setBitIndexes[i]];

		if (((andMask & unsolved) | (~orMask & unsolved)) == 0)
			break;
	}

	solved1s = andMask & unsolved;
	solved0s = ~orMask & unsolved;

	line->maskBits |= (solved1s | solved0s);
	line->partialBits |= solved1s;
	
	return;
}

void generateSubLinesConsistentPattern (Line * line)
{
	int i;
	BitSet * bSet		= line->startEdge->bitSet;
	uint64_t * perms 	= line->startEdge->permutations;
	uint64_t widthMask	= (1ULL << line->startEdge->size) - 1ULL;
	uint64_t andMask 	= widthMask;
	uint64_t orMask 	= 0ULL;
	uint64_t sizeBits	= 1ULL << line->size;
	uint64_t unsolved	= ~(line->startEdge->maskBits) & widthMask;
	uint64_t solved1s, solved0s;

	/* Looping through all valid permutations using the andMask to track which bits are always 1s
		in every permutation and the orMask for 0s. Breaks early if there are no bits that are always
		1 or 0 through all permutations */
	for (i = nextSetBit(bSet, 0); i >= 0; i = nextSetBit(bSet, i + 1))
	{
		andMask &= perms[i];
		orMask |= perms[i];

		if (((andMask & unsolved) | (~orMask & unsolved)) == 0)
			break;
	}

	solved1s = andMask & unsolved;
	solved0s = ~orMask & unsolved;

	line->startEdge->maskBits |= (solved1s | solved0s);
	line->startEdge->partialBits |= solved1s;

	bSet		= line->endEdge->bitSet;
	perms 		= line->endEdge->permutations;
	widthMask   = sizeBits - (sizeBits >> line->endEdge->size);
	andMask		= widthMask;
	orMask 		= 0ULL;
	unsolved	= ~(line->endEdge->maskBits) & widthMask;
	
	/* Looping through all valid permutations using the andMask to track which bits are always 1s
		in every permutation and the orMask for 0s. Breaks early if there are no bits that are always
		1 or 0 through all permutations */
	for (i = nextSetBit(bSet, 0); i >= 0; i = nextSetBit(bSet, i + 1))
	{
		andMask &= perms[i];
		orMask |= perms[i];

		if (((andMask & unsolved) | (~orMask & unsolved)) == 0)
			break;
	}

	solved1s = andMask & unsolved;
	solved0s = ~orMask & unsolved;

	line->endEdge->maskBits |= (solved1s | solved0s);
	line->endEdge->partialBits |= solved1s;

	return;
}

/*
 * Computes the minimum number of cells required to fit all clues
 * in the line with a 1-cell gap between each group.
 *
 * Used for overlap deduction and permutation validation.
 */
int minRequiredLength (LineClue * clueSet)
{
	int i, total = 0;

	for (i = 0; i < clueSet->clueCount; ++i)
		total += clueSet->clues[i];

	total += clueSet->clueCount - 1;

	return total;
}

/*
 * Performs overlap deduction for the Line based on left-justified and
 * right-justified clue placements.
 *
 * For each clue:
 * - Determines the guaranteed overlap between all possible placements
 * - Updates maskBits and partialBits for cells that are always filled
 *
 * TODO: Currently uses minRequiredLength; future improvements may consider
 *       clue gaps or solved cells for more advanced deductions.
 */
void overlap (Line * line)
{
	int i, j, leftEnd, rightStart;
	int leftPos = 0;
	int rightPos = line->size - minRequiredLength(line->clueSet);
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

/*
 * Determines which direction to generate permutations from.
 *
 * Parameters:
 * - line: Pointer to the Line struct to update
 *
 * The line is split in half and the number of known cells (set bits)
 * in each half of maskBits is counted. The direction with more known
 * cells is chosen to begin generation from.
 *
 * Sets:
 * - line->genDirection to DIRECTION_START or DIRECTION_END
 */
void generationDirection (Line * line)
{
	int half = line->size / 2;
	uint64_t mask = (1ULL << half) - 1ULL;

	int start = __builtin_popcountll(line->maskBits & mask);
	int end   = __builtin_popcountll(line->maskBits >> half);

	if (start >= end)
		line->genDirection = DIRECTION_START;
	else
		line->genDirection = DIRECTION_END;

	return;
}
