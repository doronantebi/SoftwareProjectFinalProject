
#ifndef SOFTWAREPROJECTFINALPROJECT_GAME_H
#define SOFTWAREPROJECTFINALPROJECT_GAME_H

#endif //SOFTWAREPROJECTFINALPROJECT_GAME_H

#include "solver.h"

static enum Mode mode = Init;

/*
 * This method is used for starting to play sudoku.
 */
void startGame();

/*
 * This function uploads a file of a game to solve.
 */
struct sudokuManager* solve(struct sudokuManager *prevBoard, char *fileName);

/*
 * This function uploads a file of a game to edit.
 */
struct sudokuManager* edit(struct sudokuManager *prevBoard, char *fileName);

/*
 * Sets the "mark errors" setting to X, where X is 0 or 1.
 */
void markErrors(struct sudokuManager *board, int X);

/*
 * Prints the board according to the given format
 */
void printBoard(struct sudokuManager *board);

/*
 * Sets in board at location [Y,X] ( X column, Y row ) value Z
 */
void set(struct sudokuManager *board, int X, int Y, int Z);

/*
 * This function validates the current board using ILP, ensuring it's solvable.
 * Valid only on Edit and Solve modes.
 */
int validate(struct sudokuManager *board);

/*
 * This function guesses a solution to the current board using LP.
 * Valid only in Solve mode.
 * X value is between 0 and 1.
 */
void guess(struct sudokuManager *board, float X);

/*
 * This function randomly creates a sudoku puzzle.
 * Randomly fills X empty cells with legal value,
 * runs ILP to solve the board,
 * and then clearing all but Y random cells.
 * Y = fixed cells #
 * Valid only in Edit mode.
 */
struct sudokuManager* generate(struct sudokuManager* prevBoard, int X, int Y);

/*
 * This function undo the previous move done by the user.
 * Valid only in Edit and Solve modes.
 */
void undo(struct sudokuManager *board);

/*
 * This function redo a move previously done by the user.
 * Valid only in Edit and Solve modes.
 */
void redo(struct sudokuManager *board);

/*
 * This function saves the board to fileName
 * Valid only in Edit and Solve modes.
 */
void save(struct sudokuManager *board, char* fileName);

/*
 * This function shows the solution to cell <X,Y>
 * Valid only in Solve mode.
 */
void hint(struct sudokuManager *board, int X, int Y);

/*
 * This function shows the solution to cell <X,Y>
 * Valid only in Solve mode.
 */
void guessHint(struct sudokuManager *board, int X, int Y);

/*
 * Prints the amount of possible solutions of the board.
 * Valid only in Edit and Solve modes.
 */
void numSolutions(struct sudokuManager *board);

/*
 * Automatically fills obvious values.
 * Valid only in Solve mode.
 */
void autofill(struct sudokuManager *board);

/*
 * Undo all moves.
 * Valid only in Edit and Solve mode.
 */
void reset(struct sudokuManager *board);

/*
 * Terminates the program
 */
void exitGame(struct sudokuManager *board);
