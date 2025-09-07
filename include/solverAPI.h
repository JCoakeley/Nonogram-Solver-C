#ifndef SOLVER_API_H
#define SOLVER_API_H

#include "../include/solver.h"
#include "../include/fileIO.h"

typedef enum {
	MODE_STANDARD,
	MODE_BENCHMARK,
	MODE_TEST
} SolvingMode;

typedef enum {
	FREE_NONE,
	FREE_LINE_CLUES,
	FREE_COLUMN_PARTIAL,
	FREE_LINES,
	FREE_SET_BIT_INDEXES
} CleanupStage;

typedef struct {
	int width, length;
	int * gameBoard;
	uint64_t rowsToUpdateBits;
	uint64_t columnsToUpdateBits;
	int * columnPartialSolution;
	int * setBitIndexes;
	LineClue ** lineClues;
	Line ** lines;
	CleanupStage stage;
} SolverContext;

int * solvePuzzle (FILE *, SolvingMode, int *, int *, int *, Timings *);

char puzzleSetup (FILE *, SolverContext *, Timings *);

void freeResources (SolverContext *);

char generationDecision (Line *, Timings *, int *, int *);

char lineGeneration (Line *, Timings *, int *, int *);

#endif
