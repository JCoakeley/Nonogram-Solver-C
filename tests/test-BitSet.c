#include <stdio.h>
#include <stdlib.h>
#include "../include/bitSet.h"

int test_newBitSet ();

int test_setAllBits ();

int main (void)
{
	int failures = 0;

	failures += test_newBitSet();

	if (failures == 0) 
	{
        printf("All tests passed!\n");
        return 0;
    } 

    else 
    {
        printf("%d tests failed.\n", failures);
        return 1;
    }
}

int test_newBitSet ()
{
	int returnValue = 0;
	BitSet * testBitSet = newBitSet(63);

	if (testBitSet->wordCount != 1)
	{
		returnValue = 1;
		printf("test_newBitSet: 1: wordCount failure\n");
	}
		
	if (testBitSet->bitCount != 63)
	{
		returnValue = 1;
		printf("test_newBitSet: 1: bitCount failure\n");
	}

	testBitSet = newBitSet(65535);

	if (testBitSet->wordCount != 1024)
	{
		returnValue = 1;
		printf("test_newBitSet: 2: wordCount failure, %d\n", testBitSet->wordCount);
	}
		
	if (testBitSet->bitCount != 65535)
	{
		returnValue = 1;
		printf("test_newBitSet: 2: bitCount failure\n");
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
		returnValue = 1;
		printf("test_setAllBits: 1: failure\n");
	}

	return returnValue;
}
