#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>
#include "../include/utility.h"

FILE * getFile (int, char *);

LineClue ** readFile (FILE *, int *, int *);

#endif
