#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "../include/solverAPI.h"

int * createGameBoard (int, int);

void printGameBoard (int *, int, int);

void setGameBoardRow (SolverContext *, int);

void setGameBoardColumn (SolverContext *, int);

void getGameBoardColumn (int *, int *, int, int, int);

char isSolved (int *, int, int);

#endif
