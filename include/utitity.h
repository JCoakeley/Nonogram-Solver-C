#include <stdlib.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0

typedef unsigned char bool;
typedef unsigned char byte;


typedef struct LineClue
{
	byte * clues;
	byte clueCount;
} LineClue;

LineClue * createLineClueStruct (int *, int);
