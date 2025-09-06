#ifndef SOLVER_H
#define SOLVER_H

#include "../include/utility.h"

Line * createLine (struct LineClue *, int, int);

SubLine * createSubLine (struct LineClue *, int);

void generatePermutations (Line *, int *);

void generatePermutationsStart (Line *, int, uint64_t, int, int *);

void generateSubLinePermutationsStart (SubLine *, int, uint64_t, char, int, int *);

int totalRemainingLengthStart (Line *, int);

int totalRemainingLengthStartSubLine (SubLine *, int);

void generatePermutationsEnd (Line *, int, uint64_t, uint64_t, int, int *);

void generateSubLinePermutationsEnd (SubLine *, int, uint64_t, uint64_t, char, int, int *);

int totalRemainingLengthEnd (Line *, int);

int totalRemainingLengthEndSubLine (SubLine *, int);

void updateBitMasks(Line *, const int *);

void updateSubLineBitMasks (Line *);

void updateBitMasksFromSubLines (Line *);

void filterPermutations (Line *);

void filterSubLinePermutations (Line *);

void generateConsistentPattern (Line *);

void generateSubLinesConsistentPattern (Line *);

int minRequiredLength (LineClue *);

void overlap (Line *);

void generationDirection (Line *);

#endif
