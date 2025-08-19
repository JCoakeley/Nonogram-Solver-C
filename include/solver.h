#ifndef SOLVER_H
#define SOLVER_H
#include <stdbool.h>
#include "../include/bitSet.h"

struct LineClue;

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

Line * createLine (struct LineClue *, int, int);

void generatePermutations (Line *, int, uint64_t, int, bool, int *);

int totalRemainingLength (Line *, int);

void updateBitMasks(Line *, const int *);

void filterPermutations (Line *);

void generateConsistentPattern (Line *);

int minRequiredLength (Line *);

void overlap (Line *);

#endif
