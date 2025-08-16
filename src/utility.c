#include "../include/utitity.h"

LineClue * createLineClueStruct(int * clueBuffer, int clueCount)
{
	int i;
	LineClue * lineClueStruct = (LineClue *)malloc(sizeof(LineClue));

	if (lineClueStruct == NULL)	return NULL;

	lineClueStruct->clueCount = clueCount;
	lineClueStruct->clues = (byte *)malloc(sizeof(byte) * clueCount);

	if (lineClueStruct->clues == NULL)
	{
		free(lineClueStruct);
		return NULL;
	}

	for (i = 0; i < clueCount; ++i)
		lineClueStruct->clues[i] = clueBuffer[i];
	
	return lineClueStruct;
}
