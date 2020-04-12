#ifndef SOFTWAREPROJECTFINALPROJECT_MAIN_AUX_H
#define SOFTWAREPROJECTFINALPROJECT_MAIN_AUX_H

#include "utilitiesBoardManager.h"

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
void printSudokuGrid(struct sudokuManager *manager, enum Mode mode);

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


/* GAME METHODS */

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
 * Some error in autofill ??????
 */
void printErrorAutofill();


/*
 * This method prints a message that the board is erronous
 */
void printBoardIsErroneous();


/*
 * the user tried to set something that is not 0 or 1 to mark_errors
 */
void printMarkErrorsInputError();

/*
 * announces the user was trying to mark errors outside of Solve mode
 */
void printMarkErrorsModeError();

/*
 * This method announces that the user has been trying to set a fixed cell
 * X is column, Y is row.
 */
void printErrorCellXYIsFixed(int X, int Y);

/*
 * This error announces that X is not a legal value, between 1 to maxVal
 */
void printErrorCellValue(int X, int maxVal);


/*
 * Error message of invalid board
 */
void printBoardNotValidError();

/*
 * Error message of unsuccessful board creation.
 */
void printErrorCreateBoard();

/*
 * Error message of illegal input value.
 */
void printGenerateInputError();

/*
 * This function prints error message if the loaded file format is illegal
 */
void printFileFormatIllegal();

/*
 * This method prints that no input was received.
 */
void printNoInput();

/*
 * This method prints that not enough numbers were entered.
 */
void printNotEnoughNumbers(int expected, int is);

/*
 * This method prints a message saying that an empty cell is set as fixed in the file loaded.
 */
void printErrorEmptyCellFixed(int row, int col);

/*
 * This function prints an error if the path for a file, given by user, is illegal.
 */
void printFilePathIllegal();

/*
 * This function prints an error that allocation failed~
 */
void printAllocFailed();

/*
 * This function prints error that
 * ((value < 0) || (x > boardLen(manager)));
 */
void printWrongRange(struct sudokuManager *board, int value);


/*
 * This function prints a message that the input is not an integer.
 */
void printNotAnInteger();

/*
 * This method prints that the board is solvable (valid).
 */
void printBoardIsValid();

#endif
