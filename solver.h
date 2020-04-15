#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef SOFTWAREPROJECTFINALPROJECT_SOLVER_H
#define SOFTWAREPROJECTFINALPROJECT_SOLVER_H

#include "utilitiesBoardManager.h"

/* VALIDATE */

/*
 * This function validates a board using ILP.
 * Return values:
 * -2: Gurobi failure.
 * -1: memory allocation failed.
 *  0: the board is invalid.
 *  1: the board is valid.
 */
int validateBoard(struct sudokuManager *manager);

/* GUESS */

/*
 * This function guesses a solution for the entire board and
 * fills its cells with values with higher probability than [threshold]
 * to appear in a solution to the board.
 * Return values:
 * -2: Gurobi failure.
 * -1: memory allocation failed.
 *  0: The board is invalid.
 *  1: The board is valid and the guessed solution is filled in retBoard.
 */
int doGuess(struct sudokuManager *manager, float threshold, int *retBoard);

/* GENERATE */

/*
 * This function raffles X cells and fill it with legal values,
 * solves the board and leave Y cells out of the solution,
 * to generate a new board.
 * It updates the new board into *retBoard.
 * Return values:
 * -1: memory allocation failed.
 *  0: generating board failed after many attempts.
 *  1: the board was successfully generated, and setted to *retBoard.
 */
int doGenerate(struct sudokuManager *prevBoard, int X, int Y, int *retBoard);

/* HINT */

/*
 * This function fills in *hint a hint for cell <row,col>.
 * Return values:
 * -2: Gurobi failure.
 * -1: memory allocation failed.
 *  0: the board is invalid.
 *  1: the board is valid and solving the board succeeded.
 */
int getHint(struct sudokuManager *manager, int row, int col, int* hint);

/* GUESS_HINT */

/*
 * This function guesses a hint for cell (row, col) using LP.
 *  Return values:
 *  -2: there was a nonfatal error because of which we can't execute
 *      the command and need to continue.
 *  -1: memory allocation failed.
 *   1: the board is solvable.
 *   0: the board is unsolvable.
 *  User must free *pCellValues iff return value == 1.
 */
int doGuessHint(struct sudokuManager *manager, int row, int col, int **pCellValues, double **pScores, int *pLength);

/* NUM_SOLUTIONS */

/*
 * This function returns the number of possible solutions of the current board
 * using the backtracking algorithm.
 * Return values:
 * -1: memory allocation failed.
 * numSolutions: returns the number of solutions to the board >= 0.
 */
int backtracking(struct sudokuManager *manager);

#endif
