#ifndef SOFTWAREPROJECTFINALPROJECT_GAME_H
#define SOFTWAREPROJECTFINALPROJECT_GAME_H

#include "solver.h"
#define LENGTH 258

/*
 * This method is used for starting to play sudoku.
 */
int startGame();

/*
 *  This function changes the value of addMarks parameter to X.
 * if (addMark == 1) erroneous cells will be marked when printing the board.
 */
void markErrors(int X);

/*
 * This function uploads a file of a game to solve.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int solve(struct sudokuManager **pPrevBoard, char *fileName);

/*
 * This function uploads a file of a game to edit.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int edit(struct sudokuManager **pPrevBoard, char *fileName);

/*
 * This function saves the board to file path "fileName".
 * Invalid board will not be saved in Edit mode.
 */
void save(struct sudokuManager *board, char* fileName);

/*
 * This function prints the board according to the given format.
 * It marks fixed cells with ".", it also marks erroneous cells with "*"
 * if the mode is "Edit" or mark_errors == 1.
 * If the board is filled, and the mode is solved,
 * it return if the board is erroneous or not.
 * If the board is legally filled, will announce the user that the board is successfully solved,
 * and change mode to Init.
 */

void printBoard(struct sudokuManager *board);

/*
 * This function sets val into the board in cell <col, row>.
 * Will not allow setting a value into a fixed cell.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int set(struct sudokuManager *board, int col, int row, int val);

/*
 * This function validates the current board using ILP, ensuring it's solvable.
 * It prints a message if the board is valid or not.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int validate(struct sudokuManager *board);

/*
 * This function guesses a solution to the current board using LP.
 * It fills cells with probability higher than X to appear in a valid solution.
 * If The board is erroneous, prints an error message.
 * X represents probability, therefore 0 <= X <= 1.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int guess(struct sudokuManager *board, float X);

/*
 * This function generates a sudoku board from the current board by:
 * filling X random cells, solving the filled board,
 * and leaving Y cells filled.
 * This function appempts to solve the board many time,
 * when it fails, it tries again.
 * If Generate fails all it's attempts, the board will remain the previous board.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int generate(struct sudokuManager **pManager, int X, int Y);

/*
 * This function undoes a move previously done by the user.
 * If there is no move to undo (the board is in it's initial state),
 * it prints an error message.
 * Each change that has been made in that board will be printed in a message.
 */
void undo(struct sudokuManager *board);

/*
 *  This function redoes a move previously undone by the user.
 * If there is no move to redo it prints an error message.
 * Each changes that has been made in that board will be printed in a message.
 */
void redo(struct sudokuManager *board);


/*
 * This function shows a possible solution to cell <col, row>.
 * If the board has several solutions, a hint for a single cell might change.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int hint(struct sudokuManager *board, int col, int row);

/*
 * This function shows the solution to cell <row,col>,
 * and prints the scored (probabilities) for each value.
 * This function prints an error if the user tries to execute it when one of the following:
 * The board is erroneous, the cell is fixed, or the cell is filled with a value.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int guessHint(struct sudokuManager *board, int col, int row);

/*
 * This function prints the amount of possible solutions of the board.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int numSolutions(struct sudokuManager *board);

/*
 * This function automatically fill "obvious" values: cells which contain only a single legal value.
 * This function will print an error when used on erroneous board.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int autofill(struct sudokuManager *board);

/*
 *  This function resets the board to its initial state by undoing all moves.
 * This function will not change the moves list, but moving the pointer to its beginning.
 */
void reset(struct sudokuManager *board);

/*
 * This function terminates the game, and frees used resources.
 * It returns 2.
 */
int exitGame(struct sudokuManager *board);

#endif
