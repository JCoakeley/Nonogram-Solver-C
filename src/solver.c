#include "../include/solver.h"
#include <stdlib.h>

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
	line->state				= LINE_ALLOC_NONE;

	return line;
}

/*
 * Recursively generates all valid permutations of a line that are consistent with
 * the current maskBits and partialBits.
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
void generatePermutations (Line * line, int clueIndex, uint64_t current, int position, char countOnly, int * permCount)
{
	int groupSize, maxStart, newPosition, start;
	uint64_t groupBits, newBits, writtenBitsMask, compareMask;

	/* Base Case: All clues placed in the permutation */
	if (clueIndex >= line->clueSet->clueCount)
	{
		/* Only counting or storing permutations that fit the mask and partial bits */
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
		if (((newBits & compareMask) ^ (line->partialBits & compareMask)) != 0)
			continue;

		generatePermutations(line, clueIndex + 1, newBits, newPosition, countOnly, permCount);
	}
}

/*
 * Computes the minimum number of cells required to place all clues starting
 * from the given clueIndex, including the required spaces between clues.
 *
 * Used to limit clue placement during permutation generation.
 */
int totalRemainingLength (Line * line, int clueIndex)
{
	int i, length = 0, size = line->clueSet->clueCount;

	for (i = clueIndex; i < size; ++i)
		length += line->clueSet->clues[i];

	length += (size - clueIndex - 1);

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
	uint64_t * const perms 	= line->permutations;
	uint64_t widthMask 		= (1ULL << line->size) - 1ULL;
	uint64_t andMask 		= widthMask;
	uint64_t orMask 		= 0ULL;
	uint64_t unsolved 		= ~(line->maskBits) & widthMask;
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

	line->maskBits |= (solved1s | solved0s);
	line->partialBits |= solved1s;
	
	return;
}

/*
 * Computes the minimum number of cells required to fit all clues
 * in the line with a 1-cell gap between each group.
 *
 * Used for overlap deduction and permutation validation.
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
