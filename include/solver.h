#ifndef SOLVER_H
#define SOLVER_H
#include "../include/utility.h"

Line * createLine (struct LineClue *, int, int);

void generatePermutations (Line *, int, uint64_t, int, bool, int *);

int totalRemainingLength (Line *, int);

void updateBitMasks(Line *, const int *);

void filterPermutations (Line *);

void generateConsistentPattern (Line *);

int minRequiredLength (Line *);

void overlap (Line *);

#endif
