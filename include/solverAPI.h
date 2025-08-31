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

int * solvePuzzle (FILE *, SolvingMode, int *, int *, Timings *);

char puzzleSetup (FILE *, SolverContext *, Timings *);

void freeResources (SolverContext *);

#endif
