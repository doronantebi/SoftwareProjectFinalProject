
#ifndef SOFTWAREPROJECTFINALPROJECT_SOLVER_H
#define SOFTWAREPROJECTFINALPROJECT_SOLVER_H

#endif //SOFTWAREPROJECTFINALPROJECT_SOLVER_H
#include "utilities.h"

/*
 * This function validates a board using ILP
 * return 1 if valid and 0 otherwise.
 */
int validateBoard(int *board, int n, int m);


/*
 * This function generates a new board with Y cells.
 * if has been successful,returns a pointer to a newBoard that will have Y values.
 * if fails, returns prevBoard
 */
struct sudokuManager* doGenerate(struct sudokuManager *prevBoard, struct sudokuManager *newBoard, int X, int Y);