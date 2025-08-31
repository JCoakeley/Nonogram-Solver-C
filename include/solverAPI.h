#ifndef SOLVER_API_H
#define SOLVER_API_H

#include <stdio.h>
#include "../include/solver.h"

typedef enum {
	MODE_STANDARD,
	MODE_BENCHMARK,
	MODE_TEST
} SolvingMode;

typedef enum {
	FREE_NONE,
	FREE_LINE_CLUES,
	FREE_ROWS,
	FREE_COLUMNS,
	FREE_COLUMN_PARTIAL,
	FREE_LINES
} CleanupStage;

typedef struct {
	int width, length;
	int * gameBoard;
	int * rowsToUpdate;
	int * columnsToUpdate;
	int * columnPartialSolution;
	LineClue ** lineClues;
	Line ** lines;
	CleanupStage stage;
} SolverContext;

int * solvePuzzle (FILE *, SolvingMode, int *);

bool puzzleSetup (FILE *, SolverContext *);

void freeResources (SolverContext *);

#endif
