#ifndef UTIL_H
#define UTIL_H
#include "../include/solver.h"

#define TRUE 1
#define FALSE 0

typedef struct LineClue {
	int * clues;
	int clueCount;
} LineClue;

LineClue * createLineClueSet (int *, int);

void printLineDetails (Line *);

#endif
