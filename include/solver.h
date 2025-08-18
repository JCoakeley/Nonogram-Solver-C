#ifndef SOLVER_H
#define SOLVER_H
#include "../include/utility.h"
#include "../include/bitSet.h"

typedef struct Line {
	int lineId;
	int permutationCount;
	int size;
	BitSet * bitSet;
	LineClue * clues;
	uint64_t * permutations;
	uint64_t partialBits;
	uint64_t maskBits;
} Line;

Line * createLine (LineClue *, int, int);

void updateBitMasks(Line *, const int *);

void filterPermutations (Line *);

void generateConsistantPattern (Line *);

int minRequiredLength (Line *);

void overlap (Line *);

#endif
