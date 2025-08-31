#include "../include/bitSet.h"
#include <stdlib.h>
#include <string.h>

/*
 * Creates and initializes a new BitSet with enough words to cover the given bit count.
 *
 * Returns:
 * - Pointer to a newly allocated BitSet with all bits set to 1 (except unused bits).
 * - NULL if memory allocation fails.
 *
 * The caller is responsible for checking for NULL.
 */
BitSet * newBitSet (int bitCount)
{
	BitSet * bitSet = (BitSet *)malloc(sizeof(BitSet));
	if (bitSet == NULL) return bitSet;

	bitSet->bitCount = bitCount;

	/* Compute number of 64-bit words needed to store bitCount bits */
	bitSet->wordCount = bitCount >> 6;
	if ((bitCount & 63) != 0) bitSet->wordCount++;

	bitSet->words = (uint64_t *)malloc(sizeof(uint64_t) * bitSet->wordCount);

	if (bitSet->words == NULL)
	{
		free(bitSet);
		bitSet = NULL;
	}

	else
		setAllBits(bitSet);
	
	return bitSet;
}

/*
 * Sets all usable bits in the BitSet to 1.
 *
 * Any unused bits in the final word (beyond bitCount) are cleared to 0
 * to ensure safety and correctness during filtering.
 */
void setAllBits (BitSet * bitSet)
{
	int unsetCount = bitSet->bitCount & 63;

	/* Set all bits in each word to 1 initially */
	memset(bitSet->words, 0xFF, bitSet->wordCount * sizeof(uint64_t));

	/* Clear bits beyond bitCount in the final word (ensure unused bits are 0) */
	if (unsetCount > 0)
		bitSet->words[bitSet->wordCount - 1] &= (1ULL << unsetCount) - 1;

	return;
}

/*
 * Clears a specific bit in the BitSet (sets it to 0).
 *
 * Arguments:
 * - bitSet: The BitSet to modify.
 * - bit: Index of the bit to clear (0-based).
 */
void clearBit (BitSet * bitSet, int bit)
{
	uint64_t * wordPtr = bitSet->words;
	int wordNum = bit >> 6, bitNum = bit & 63;

	/* Create mask to clear a single bit by ANDing with its inverse */
	uint64_t mask = 1ULL << bitNum;
	uint64_t inverted = ~mask;

	wordPtr += wordNum;
	*wordPtr &= inverted;
	
	return;
}

/*
 * Returns the index of the next set bit (1) in the BitSet starting from the given index.
 *
 * If no set bits remain after the starting index, returns -1.
 * This function performs bit masking to skip already-cleared bits efficiently.
 *
 * Arguments:
 * - bitSet: The BitSet to search.
 * - startingBit: The index to start the search from.
 */
int nextSetBit (BitSet * bitSet, int startingBit)
{
	uint64_t targetWord;
	int wordNum, bitNum, bitIndex = -1;
	if (startingBit >= bitSet->bitCount) return -1;
	
	wordNum = startingBit >> 6;
	bitNum = startingBit & 63;
	
	/* Mask out bits before startingBit in the current word */
	targetWord = bitSet->words[wordNum] & (~0ULL << bitNum);

	if (targetWord != 0)
	{
		/* Use builtin to find position of least-significant set bit */
		bitIndex = (wordNum << 6) + __builtin_ctzll(targetWord);
		return bitIndex;
	}
	++wordNum;

	/* Search remaining words until a set bit is found or end is reached */
	while (wordNum < bitSet->wordCount)
	{
		targetWord = bitSet->words[wordNum];

		if (targetWord != 0)
		{
			/* Use builtin to find position of least-significant set bit */
			bitIndex = (wordNum << 6) + __builtin_ctzll(targetWord);
			return bitIndex;
		}
		
		++wordNum;
	}
	
	return bitIndex;
}
