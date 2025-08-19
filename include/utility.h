#ifndef UTIL_H
#define UTIL_H

#define TRUE 1
#define FALSE 0

typedef unsigned char bool;

typedef struct LineClue {
	int * clues;
	int clueCount;
} LineClue;

LineClue * createLineClueSet (int *, int);

#endif
