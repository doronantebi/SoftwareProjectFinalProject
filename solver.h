#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef SOFTWAREPROJECTFINALPROJECT_SOLVER_H
#define SOFTWAREPROJECTFINALPROJECT_SOLVER_H

#include "utilitiesBoardManager.h"

/*
 * This function validates a board using ILP
 * return 1 if valid and 0 otherwise.
 */
int validateBoard(struct sudokuManager *manager);


/*
 * This function generates a new board with Y cells.
 * if has been successful,returns a pointer to a newBoard that will have Y values.
 * if fails, returns prevBoard
 */
struct sudokuManager* doGenerate(struct sudokuManager *prevBoard, int *newBoard, int X, int Y);


double randRangeDouble(double min, double max);

#endif