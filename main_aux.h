#ifndef SOFTWAREPROJECTFINALPROJECT_MAIN_AUX_H
#define SOFTWAREPROJECTFINALPROJECT_MAIN_AUX_H

#endif //SOFTWAREPROJECTFINALPROJECT_MAIN_AUX_H

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
void printSudokuGrid(struct sudokuManager *manager);


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
 * the user tried to set something that is not 0 or 1 to mark_errors
 */
void printMarkErrorsInputError();

/*
 * announces the user was trying to mark errors outside of Solve mode
 */
void printMarkErrorsModeError();

/*
 * This method announces that the user has been trying to set a fixed cell
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