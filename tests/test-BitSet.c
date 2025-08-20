#include "test-BitSet.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/bitSet.h"

int test_newBitSet ()
{
	int returnValue = 0;
	BitSet * testBitSet = newBitSet(63);

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

	testWord = *(testBitSet->words);

	if (testWord != compareWord)
	{
		++returnValue;
		printf("Test Failure: test_setAllBits #1: No match\n");
	}

	return returnValue;
}

int test_clearBit ()
{
	int returnValue = 0;

	BitSet * testBitSet = newBitSet(10);

	if (testBitSet->words[0] != 1023ULL)
		returnValue = 1;

	clearBit(testBitSet, 1);

	if (testBitSet->words[0] != 1021ULL)
		returnValue = 1;

	clearBit(testBitSet, 1);
	
	if (testBitSet->words[0] != 1021ULL)
		returnValue = 1;

	clearBit(testBitSet, 3);
		
	if (testBitSet->words[0] != 1013ULL)
		returnValue = 1;

	clearBit(testBitSet, 7);
		
	if (testBitSet->words[0] != 885ULL)
		returnValue = 1;

	if (returnValue == 1)
		printf("Test Failure: test_clearBit #1\n");

	return returnValue;
}

int test_nextSetBit ()
{
	int setBit = 0, returnValue = 0;

	BitSet * testBitSet = newBitSet(10);
	clearBit(testBitSet, 1);
	clearBit(testBitSet, 3);
	clearBit(testBitSet, 7);
	clearBit(testBitSet, 6);
	clearBit(testBitSet, 9);

	setBit = nextSetBit(testBitSet, setBit);

	if (setBit != 0)
		returnValue = 1;

	setBit = nextSetBit(testBitSet, setBit + 1);
	
	if (setBit != 2)
		returnValue = 1;

	setBit = nextSetBit(testBitSet, setBit + 1);
		
	if (setBit != 4)
		returnValue = 1;

	setBit = nextSetBit(testBitSet, setBit + 1);
			
	if (setBit != 5)
		returnValue = 1;

	setBit = nextSetBit(testBitSet, setBit + 1);
				
	if (setBit != 8)
		returnValue = 1;

	setBit = nextSetBit(testBitSet, setBit + 1);
						
	if (setBit != -1)
		returnValue = 1;

	if (returnValue == -1)
		printf("Test Failure: test_nextSetBit\n");	

	return returnValue;
}
