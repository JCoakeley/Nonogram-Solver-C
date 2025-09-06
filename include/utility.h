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

typedef enum {
	DIRECTION_NONE,
	DIRECTION_START,
	DIRECTION_END
} GenerationDirection;

typedef struct LineClue {
	int * clues;
	int clueCount;
} LineClue;

typedef struct SubLine {
	int size;
	int lineSize;
	int permCount;
	int storeCount;
	BitSet * bitSet;
	struct LineClue * clueSet;
	uint64_t * permutations;
	uint64_t partialBits;
	uint64_t maskBits;
	LineAllocState state;
} SubLine;

typedef struct Line {
	int lineId;
	int storeCount;
	int size;
	BitSet * bitSet;
	LineClue * clueSet;
	SubLine * startEdge;
	SubLine * endEdge;
	uint64_t * permutations;
	uint64_t partialBits;
	uint64_t maskBits;
	uint64_t maxPermutations;
	LineAllocState state;
	GenerationDirection genDirection;
} Line;

LineClue * createLineClueSet (int *, int);

void printLineDetails (Line *);

void printTimingData (Timings *);

void printFormattedTime (long);

uint64_t nCr (int, int);

void formatNumber (char *, int);

#endif
