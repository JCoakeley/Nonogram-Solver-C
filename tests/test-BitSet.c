#include "test-BitSet.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/bitSet.h"

int test_newBitSet ()
{
	int returnValue = 0;
	BitSet * testBitSet = newBitSet(63);

	printf("test1\n");
	if (testBitSet->wordCount != 1)
	{
		++returnValue;
		printf("Test Failure: test_newBitSet #1: Expected: 1, Actual: %d\n", testBitSet->wordCount);
	}
		
	if (testBitSet->bitCount != 63)
	{
		++returnValue;
		printf("Test Failure: test_newBitSet #1: Expected: 63, Actual: %d\n", testBitSet->bitCount);
	}

	testBitSet = newBitSet(65535);

	printf("test2\n");
	
	if (testBitSet->wordCount != 1024)
	{
		++returnValue;
		printf("Test Failure: test_newBitSet #1: Expected: 1024, Actual: %d\n", testBitSet->wordCount);
	}
		
	if (testBitSet->bitCount != 65535)
	{
		++returnValue;
		printf("Test Failure: test_newBitSet #1: Expected: 65535, Actual: %d\n", testBitSet->bitCount);
	}

	return returnValue;
}

int test_setAllBits ()
{
	int returnValue = 0;
	uint64_t  testWord, compareWord = (1ULL << 32) - 1;
	BitSet * testBitSet = newBitSet(32);

	setAllBits(testBitSet);
	testWord = *(testBitSet->words);

	if (testWord != compareWord)
	{
		++returnValue;
		printf("Test Failure: test_setAllBits #1: No match\n");
	}

	return returnValue;
}
