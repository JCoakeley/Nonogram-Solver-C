#ifndef GAMEBOARD_H
#define GAMEBOARD_H

struct Line;

int * createGameBoard (int, int);

void printGameBoard (int *, int, int);

void setGameBoardRow (int *, struct Line *, int *);

void setGameBoardColumn (int *, struct Line *, int, int *);

void getGameBoardColumn (int *, int *, int, int, int);

char isSolved (int *, int, int);

#endif
