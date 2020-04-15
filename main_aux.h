#ifndef SOFTWAREPROJECTFINALPROJECT_MAIN_AUX_H
#define SOFTWAREPROJECTFINALPROJECT_MAIN_AUX_H

#include "utilitiesBoardManager.h"

/* GENERAL GAME RELATED METHODS */

/*
 * This method prints the game title
 */
void printGameTitle();

/*
 * This message prints a message that the board is solved successfully!
 */
void printBoardIsSolved();

/*
 * This function prints the exit message.
 */
void printExitMessage();

/*
 * This method prints the users board
 */
void printSudokuGrid(struct sudokuManager *manager, enum Mode mode, int addMarks);


/* PARSER METHODS*/

/*
 * This method receives a word and returns a number unique to the
 * command if it is a legal command, or -1 if it is an invalid command.
 * It returns the index of the command in the array commandList.
 */
int commandNum (char* word);

/*
 * The function prints a message to the user saying
 * that not enough parameters were entered.
 */
void printFewParams(int numOfParams, int indexCommand);

/*
 * The function prints a message to the user saying that too many parameters were entered.
 * There are some possibilities for a valid number of parameters, all of which are in the array arrNumOfParams.
 */
void printExtraParamsExtend(int *arrNumOfParams, int len, int indexCommand);

/*
 * The function prints a message to the user saying that too many parameters were entered.
 * Assumes only one possibility for a valid number of parameters.
 */
void printExtraParams(int numOfParams, int indexCommand);

/*
 * This method returns a string description of the mode of the game.
 */
char* modeToString(enum Mode mode);

/*
 * The function prints a message to the user saying that the command is not available in the current mode.
 * It receives an array of modes in which the command is available and its length.
 */
void printUnavailableMode(int indexCommand, enum Mode mode, enum Mode *availableModes, int length);

/*
 * This method prints a message to the user saying that the value entered is not in the correct range.
 */
void printWrongRangeInt(int indexCommand, int value, int indexParam);

/*
 * This method prints a message to the user saying that
 * the value entered is not in the correct range.
 */
void printWrongRangeFloat(int indexCommand, float value, int indexParam);

/*
 * This method prints a message to the user saying that
 * the received parameter is not a number.
 */
void printNotANumber(int indexParam);

/*
 * This method prints a message to the user saying that
 * the received parameter is not a number.
 */
void printNotAFloat(int indexParam);

/*
 * This method prints a message to the user saying that the command entered does not exist.
 */
void printInvalidCommand();

/*
 * This function prints an error message if the user calls "generate"
 * with X that is larger than the amount of empty cells in the board.
 * X > emptyCells.
 */
void printGenerateInputError(int emptyCells, int X);

/* END OF PARSER METHODS*/

/* ERRORS */

/* MOVES LIST RELATED */

/*
 * This method announces that there is no more next moves,
 * can be called if the user tries to "Redo".
 */
void printNoNextMoveError();

/*
 * This method announces that there is no more next moves, can be called
 * if the user tries to "undo" of functions that uses it (e.g. "reset").
 */
void printNoPrevMoveError();

/*
 * This function prints a change in a cell in the board.
 * will be called from undo and redo.
 */
void printActionWasMade(int row, int col, int prevVal, int newVal);

/* BOARD */

/*
 * This function prints a message that the board is erroneous.
 * Can be called when scanning an erroneous board from file (via: "solve"),
 * or when calling one of the following functions:
 * "autofill", "validate", "guess", "edit", and "guessHint".
 */
void printBoardIsErroneous();

/*
 * This function prints an error when the user tries
 * to change the value of a fixed cell.
 * Can be called from "set", "hint" and "guessHint".
 * The format of cell print is <col, row>, similar to "set".
 */
void printErrorCellXYIsFixed(int row, int col);

/*
 * This function prints an error when the user tries
 * to receive a hint for a cell that contains a value.
 * The format of cell print is <col, row>, similar to "set".
 */
void printErrorCellContainsValue(int row, int col);

/* GUROBI RELATED PRINTS */

/*
 * This function is called if Gurobi process has failed.
 */
void printGurobiFailedTryAgain();

/*
 * This function is called if the user called "validate",
 * and the board turned out to be invalid.
 */
void printBoardNotValidError();

/*
 * This function is called if the user called "validate",
 * and the board turned out to be valid.
 */
void printBoardIsValid();

/*
 * This function prints a message that function "generate" has failed
 * solving the board in all its attempts.
 */
void printGenerateFailed();

/*
 * This function prints the amount of possible solutions of the board,
 * returned by "numSolutions".
 */
void printNumOfSolutions(int num);


/*
 * This function prints the possible values for a cell,
 * calculated by "guessHint" function for a single cell.
 * The format of cell print is <col, row>, similar to "set".
 */
void printValuesAndScores(int row, int col, int *cellValues, double *scores, int length);

/*
 * This function prints the hint for the cell, as the user requested.
 * The format of cell print is <col, row>, similar to "set".
 */
void printHint(int row, int col, int hint);

/* FILES RELATED */

/*
 * This function prints a message to the user if
 * the given file contains more characters than expected,
 * i.e. the format if the file is illegal.
 */
void printTooLongFile();

/*
 * This function prints an error to the user if the file path is illegal.
 */
void printFilePathIllegal();

/*
 * This function prints a message that a number read from a file
 * is not in the correct range for the game.
 */
void printWrongRangeFile(int number, int start, int end);

/*
 * This function prints a message that the input is not an integer.
 */
void printNotAnInteger();

/*
 * This function prints a message if an empty cell is set
 * as fixed by the addition of "." to the value in the file loaded.
 * The format of cell print is <col, row>, similar to "set".
 */
void printErrorEmptyCellFixed(int row, int col);

/*
 * This function prints that no input was received.
 */
void printNoInput();

/*
 * This function prints that not enough numbers were entered.
 */
void printNotEnoughNumbers();

/* MEMORY ALLOCATION FAILURE */

/*
 * This function prints an error that allocation failed~
 */
void printAllocFailed();


#endif
