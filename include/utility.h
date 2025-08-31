#ifndef UTIL_H
#define UTIL_H

#include "../include/bitSet.h"
#include "../include/timing.h"

#define TRUE 1
#define FALSE 0

typedef enum {
	LINE_ALLOC_NONE,
	LINE_ALLOC_PERMS,
	LINE_ALLOC_ALL
} LineAllocState;

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
	LineAllocState state;
} Line;

LineClue * createLineClueSet (int *, int);

void printLineDetails (Line *);

void printTimingData (Timings *);

void printFormattedTime (long);

#endif
