#ifndef UTIL_H
#define UTIL_H
#include <stdbool.h>
#include "../include/bitSet.h"

#define TRUE 1
#define FALSE 0

typedef struct LineClue {
	int * clues;
	int clueCount;
} LineClue;

typedef struct Line {
	int lineId;
	int permutationCount;
	int storeCount;
	int size;
	BitSet * bitSet;
	struct LineClue * clueSet;
	uint64_t * permutations;
	uint64_t partialBits;
	uint64_t maskBits;
} Line;

LineClue * createLineClueSet (int *, int);

void printLineDetails (Line *);

#endif
