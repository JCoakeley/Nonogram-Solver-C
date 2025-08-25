#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "../include/utility.h"

int * createGameBoard (int, int);

void printGameBoard (int *, int, int);

void setGameBoardRow (int *, Line *, int *);

void setGameBoardColumn (int *, Line *, int, int *);

void getGameBoardColumn (int *, int *, int, int, int);

bool isSolved (int *, int, int);

#endif
