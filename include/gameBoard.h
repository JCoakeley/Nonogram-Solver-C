#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#include "../include/utility.h"

int * createGameBoard (int, int);

void printGameBoard (int *, int, int);

int * setGameBoardRow (int *, int *, int, int, int *);

int * setGameBoardColumn (int *, int *, int, int, int, int *);

bool isSolved (int *, int, int);

#endif
