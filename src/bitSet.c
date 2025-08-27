#include "../include/bitSet.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
* Creates a bitSet of the minimum size to accomodate the specified bit count.
* Will return a pointer to the created bit Set, will return NULL if there are
* errors alocating memory, it is the responsibility of the calling function to
* act on this error.
*/
BitSet * newBitSet (int bitCount)
{
	BitSet * bitSet = (BitSet *)malloc(sizeof(BitSet));
	if (bitSet == NULL) return bitSet;

	bitSet->bitCount = bitCount;
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
* Sets all bits of the provided bitSet to 1 except those past the bitCount
* of the bitSet. These are unused bits of the bitSet and they are set to 0 
* for safety.
*/
void setAllBits (BitSet * bitSet)
{
	int unsetCount = bitSet->bitCount & 63;
	
	memset(bitSet->words, 0xFF, bitSet->wordCount * sizeof(uint64_t));

	/* Clearing the bits of the last word that are past the bitCount of the bitSet */
	if (unsetCount > 0)
		bitSet->words[bitSet->wordCount - 1] &= (1ULL << unsetCount) - 1;

	return;
}

/*
* Clears the value at the specified bit of the specified bitSet, sets it to 0.
*/
void clearBit (BitSet * bitSet, int bit)
{
	uint64_t * wordPtr = bitSet->words;
	int wordNum = bit >> 6, bitNum = bit & 63;
	uint64_t mask = 1ULL << bitNum;
	uint64_t inverted = ~mask;

	wordPtr += wordNum;
	*wordPtr &= inverted;
	
	return;
}

/*
* Starting from the bit index provided searches for the next set bit and returns
* the index of it. If the are no set bits remaining past the provided index then
* -1 is returned.
*/
int nextSetBit (BitSet * bitSet, int startingBit)
{
	uint64_t targetWord;
	int wordNum, bitNum, bitIndex = -1;
	if (startingBit >= bitSet->bitCount) return -1;
	
	wordNum = startingBit >> 6;
	bitNum = startingBit & 63;
	
	/* Masking any bits in the word before the specified index */
	targetWord = bitSet->words[wordNum] & (~0ULL << bitNum);

	if (targetWord != 0)
	{
		bitIndex = (wordNum << 6) + __builtin_ctzll(targetWord);
		return bitIndex;
	}
	++wordNum;

	/* Iterating through each word of the bitSet until one has a set
		bit or all words are searched. */
	while (wordNum < bitSet->wordCount)
	{
		targetWord = bitSet->words[wordNum];

		if (targetWord != 0)
		{
			bitIndex = (wordNum << 6) + __builtin_ctzll(targetWord);
			return bitIndex;
		}
		
		++wordNum;
	}
	
	return bitIndex;
}
