#ifndef SOFTWAREPROJECTFINALPROJECT_MAIN_AUX_H
#define SOFTWAREPROJECTFINALPROJECT_MAIN_AUX_H

#include "utilitiesBoardManager.h"

/* GENERAL GAME RELATED METHODS */

/*
 * This method prints the game title
 */
void printGameTitle();


/*
 * This function prints the exit message.
 */
void printExitMessage();


/*
 * This method prints the users board
 */
void printSudokuGrid(struct sudokuManager *manager, enum Mode mode, int addMarks);

/*
 * This message prints a message that the board is solved successfully!
 */
void printBoardIsSolved();


/* PARSER METHODS*/

/*
 * This method receives a word and returns a number unique to the
 * command if it is a legal command, or -1 if it is an invalid command.
 * It returns the index of the command in the array commandList.
 */
int commandNum (char* word);

/*
 * The function prints a message to the user saying that not enough parameters were entered.
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
 * This method returns a string description of mode.
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
 * This method prints a message to the user saying that the value entered is not in the correct range.
 */
void printWrongRangeFloat(int indexCommand, float value, int indexParam);

/*
 * This method prints a message to the user saying that the received parameter is not a number.
 */
void printNotANumber(int indexParam);

/*
 * This method prints a message to the user saying that the received parameter is not a number.
 */
void printNotAFloat(int indexParam);

/*
 * This method prints a message to the user saying that there are not enough empty cells.
 */
void printNotEnoughEmptyCells(int emptyCells, int requiredEmptyCells);

/*
 * This method prints a message to the user saying that the command entered does not exist.
 */
void printInvalidCommand();

/* END OF PARSER METHODS*/


/* ERRORS */

/* MOVES LIST RELATED */

/*
 * This method announces that there is no more next moves,
 * can be called if the user tries to "Redo".
 */
void printNoNextMoveError();

/*
 * This method announces that there is no more next moves,
 * can be called if the user tries to "Redo".
 */
void printNoPrevMoveError();

/*
 * This function prints a change in a cell in the board.
 * will be called from undo and redo.
 */
void printActionWasMade(int row, int col, int prevVal, int newVal);

/* BOARD */

/*
 * This method prints a message that the board is erronous
 */
void printBoardIsErroneous();

/*
 * This method announces that the user has been trying to set a fixed cell
 * X is column, Y is row.
 */
void printErrorCellXYIsFixed(int col, int row);

/*
 * This message is printed when trying to call hint with an erroneous cell
 */
void printErrorCellIsErroneous(int col, int row);

/*
 * This message is printed when trying to call hint with a cell that contains a value
 */
void printErrorCellContainsValue(int col, int row);

/* VALIDATE */

/*
 * This message is printed if the gurobi has failed.
 */
void printGurobiFailedTryAgain();

/*
 * Error message of invalid board
 */
void printBoardNotValidError();

/*
 * This method prints that the board is solvable (valid).
 */
void printBoardIsValid();

/*
 * Error message of illegal input value.
 */
void printGenerateInputError();

/*
 * This method prints a message to the user saying how many possible values there are.
 */
void printNumOfSolutions(int num);


/*
 * prints the array of possible legal values for guess hint
 */
void printArray(int *cellValues, int length);

/* FILES RELATED */

/*
 * This method prints a message to the user saying that the file is too long than expected.
 */
void printTooLongFile();

/*
 * This function prints an error if the path for a file, given by user, is illegal.
 */
void printFilePathIllegal();

/*
 * This method prints that the number read from a file is not in the correct range.
 */
void printWrongRangeFile(int number, int start, int end);


/*
 * This function prints a message that the input is not an integer.
 */
void printNotAnInteger();

/*
 * This method prints a message saying that an
 * empty cell is set as fixed in the file loaded.
 */
void printErrorEmptyCellFixed(int row, int col);


/*
 * This method prints that no input was received.
 */
void printNoInput();

/*
 * This method prints that not enough numbers were entered.
 */
void printNotEnoughNumbers();

/*
 * This method prints a message saying that an
 * empty cell is set as fixed in the file loaded.
 */
void printErrorEmptyCellFixed(int row, int col);

/* MEMORY ALLOCATION FAILURE */

/*
 * This function prints an error that allocation failed~
 */
void printAllocFailed();






#endif
