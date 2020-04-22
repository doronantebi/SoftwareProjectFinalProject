/*
 * This module is meant to deal everything that has to do with Gurobi.
 * It creates variables and constraints meant to solve the sudoku board in different ways using Gurobi
 * and has functions that use this solution in several ways.
 */

#ifndef SOFTWAREPROJECTFINALPROJECT_GUROBI_H
#define SOFTWAREPROJECTFINALPROJECT_GUROBI_H

#include "utilities_board_manager.h"
#include "main_aux.h"
#include "solver.h"

typedef enum {
    BINARY = 0,
    INTEGER = 1,
    CONTINUOUS = 2
} GurobiOption;

/*
 * This method solves the current board using ILP.
 * The solution is returned through retBoard.
 *  Return values:
 * -2: memory allocation failed.
 * -1: Gurobi had an error.
 *  0: the board is invalid.
 *  1: the board is valid, and *retBoard is filled with a solution.
 */
int solveBoard(struct sudokuManager *manager, int **retBoard);

/*
 * This function solves the current board using LP.
 *  Return values:
 * -2: memory allocation failed.
 * -1: Gurobi had an error.
 *  0: the board is invalid.
 *  1: cell <row, col> values were successfully guessed.
 *  It returns all the possible values of cell <row, col> through *pCellValues and their scores through *pScores.
 * The length of *pCellValues and *pScores is returned through *pLength.
 * User needs to free *pCellValues and *pScores iff return value == 1.
*/
int guessCellValues(struct sudokuManager *manager, int row, int col,
                    int **pCellValues, double **pScores, int *pLength);

/*
 * This function solves the current board using LP.
 * retBoard is a copy of manager->board.
 * It fills retBoard with the solution guessed iff return value == 1.
 * Return values:
 * -2: memory allocation failed.
 * -1: Gurobi had an error.
 *  0: the board is invalid.
 *  1: the board was successfully guessed.
 */
int guessSolution(struct sudokuManager *manager,
                  float threshold, int *retBoard);

#endif
