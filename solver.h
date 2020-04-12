#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef SOFTWAREPROJECTFINALPROJECT_SOLVER_H
#define SOFTWAREPROJECTFINALPROJECT_SOLVER_H

#include "utilitiesBoardManager.h"


/*
 * This method returns the number of possible solutions of the current board using the backtracking algorithm.
 * If there is memory allocation error, it returns -1.
 */
int backtracking(struct sudokuManager *manager);

/*
 * This function validates a board using ILP
 * return 1 if valid and 0 otherwise.
 */
int validateBoard(struct sudokuManager *manager);


/*
 * This function fills in *hint a hint for cell <row ,col>
 * returns 1 if succeed solving the board.
 * return -1 if allocate failed
 * returns 0 if board could not be solved or gurobi failed
 */
int getHint(struct sudokuManager *manager, int row, int col, int* hint);

/*
 * This function generates a new board with Y cells.
 * if has been successful,returns a pointer to a newBoard that will have Y values.
 * if fails, returns prevBoard
 */
int* doGenerate(struct sudokuManager *prevBoard, int *newBoard, int X, int Y);



/*
 * This method guesses a hint for cell (row, col) using LP.
 *  The method returns -1 there was a nonfatal error because of which we can't execute the command and need to continue,
 *  -2 if memory allocation failed, and 0 otherwise.
 *  User needs to free *pCellValues iff return value == 0.
 */
int doGuessHint(struct sudokuManager *manager, int row, int col, int **pCellValues, int *pLength);


/*
 * This method guesses a solution for the entire board and fills its cells with this solution if legal.
 * The method returns -1 there was a nonfatal error because of which we can't execute the command and need to continue,
 *  -2 if memory allocation failed, and 0 otherwise.
 */
int doGuess(struct sudokuManager *manager, float threshold);

double randRangeDouble(double min, double max);

#endif
