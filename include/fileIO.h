#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>

struct LineClue;

FILE * getFile (int, char *);

struct LineClue ** readFile (FILE *, int *, int *);

#endif
