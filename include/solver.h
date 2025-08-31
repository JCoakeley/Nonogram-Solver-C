#ifndef SOLVER_H
#define SOLVER_H

#include "../include/utility.h"

Line * createLine (struct LineClue *, int, int);

void generatePermutations (Line *, char, int *);

void generatePermutationsStart (Line *, int, uint64_t, int, char, int *);

int totalRemainingLengthStart (Line *, int);

void generatePermutationsEnd (Line *, int, uint64_t, uint64_t, int, char, int *);

int totalRemainingLengthEnd (Line *, int);

void updateBitMasks(Line *, const int *);

void filterPermutations (Line *);

void generateConsistentPattern (Line *);

int minRequiredLength (LineClue *);

void overlap (Line *);

void generationDirection (Line *);

#endif
