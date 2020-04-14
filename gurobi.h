
#include "utilitiesBoardManager.h"
#include "main_aux.h"

#include "solver.h"

#ifndef SOFTWAREPROJECTFINALPROJECT_GUROBI_H
#define SOFTWAREPROJECTFINALPROJECT_GUROBI_H

typedef enum {
    BINARY = 0,
    INTEGER = 1,
    CONTINUOUS = 2
} GurobiOption;



/*
 * This method solves the current board using ILP.
 * The solution is returned through retBoard.
 * The method returns -1 if Gurobi had an error,
 * -2 if memory allocation failed
 * 1 if *retBoard is filled with a solution (the board is solvable),
 * and 0 if the board is unsolvable.
 */
int solveBoard(struct sudokuManager *manager, int **retBoard);

/* This method solves the current board using LP.
* The method returns -1 if Gurobi had an error,
 * -2 if memory allocation failed,
 * 1 if we succeeded in guessing the values (the board is solvable),
 * and 0 if the board is unsolvable.
 * It returns all the possible values of cell (row, col) through *pCellValues, and its length through *pLength.
 * User needs to free *pCellValues iff return value == 1.
*/
int guessCellValues(struct sudokuManager *manager, int row, int col,
                    int **pCellValues, double **pScores, int *pLength);

/* This method solves the current board using LP.
* The method returns -1 if Gurobi had an error,
 * -2 if memory allocation failed,
 * 1 if we succeeded in guessing the values (the board is solvable),
 * and 0 if the board is unsolvable.
 * It fills the board with the solution guessed iff return value == 1.
*/
int guessSolution(struct sudokuManager *manager,
                  float threshold);


#endif
