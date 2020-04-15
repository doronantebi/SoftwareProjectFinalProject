#include <stdio.h>
#include "main_aux.h"
#include <string.h>

#define title1 "                              ,,                               \n .M\"\"\"bgd                   `7MM              `7MM                  \n,MI    \"Y                     MM                MM                  \n`MMb."
#define title2    "     `7MM  `7MM     ,M\"\"bMM    ,pW\"Wq.     MM  ,MP'  `7MM  `7MM  \n  `YMMNq.   MM    MM   ,AP    MM   6W'   `Wb    MM ;Y       MM    MM  \n.     `MM   MM    MM   8MI    MM   8M     M8    MM;Mm       MM    MM  \nMb     dM   MM    MM   `Mb    MM   YA.   ,A9    MM `Mb.     MM    MM  \nP\"Ybmmd\"    `Mbod\"YML.  `Wbmd\"MML.  `Ybmd9'.   JMML. YA.    `Mbod\"YML."
#define creators "Din Moshe and Doron Antebi"

#define commandListLength 17
static char* commandList[] = {"solve", "edit", "mark_errors", "print_board", "set", "validate",
                              "guess", "generate", "undo", "redo", "save", "hint", "guess_hint",
                              "num_solutions", "autofill", "reset", "exit"};


/* GENERAL GAME RELATED METHODS */

/*
 * This function prints the title of the game.
 */
void printGameTitle(){
    printf("%s%s\n\n             Creators: %s.\n\n", title1, title2, creators);
}

/*
 * this function prints an announcement that the user have solved the game.
 */
void printBoardIsSolved(){
    printf("Congratulations! The board is successfully solved!\n");
}

/*
 * This function prints an exit message once the user asks to exit the game.
 */
void printExitMessage(){
    printf("Bye bye! (:\n");
}

/* BOARD PRINT AUXILIARY FUNCTIONS */

/*
 * This function prints the cell (row, col).
 * Prints "." after the cell value if the cell is fixed.
 * Prints "*" after the cell value if it is erroneous.
 * Prints whitespaces if the cell does not contain value (value is 0).
 */
void printCell(struct sudokuManager *manager, int row, int col, enum Mode mode, int addMarks){
    int addErrors = 0;
    int index = matIndex(manager->m, manager->n, row, col);

    if (addMarks == 1 || mode == Edit){
        addErrors = 1; /* Errors will be marked with "*" */
    }
    if(isFixedCell(manager, row, col)){
        printf(" %2d.",manager->board[index]);
        return;
    }
    /* Can not be fixed */
    if(isErroneous(manager, row, col) && addErrors){
        printf(" %2d*",manager->board[index]);
    }
    else { /* cell is not fixed nor erroneous */
        if(manager->board[index] == 0){
            printf("    ");
        } else{
            printf(" %2d ",manager->board[index]);
        }
    }
}

/*
 * This function prints a line separator for the blocks of the board.
 * For our purposes; length must be equal to 4N+m+1.
 */
void printSeparatorRow(int length){
    int i;
    for (i = 0; i < length ; i++) {
        printf("-");
    }
    printf("\n");
}

/*
 * This function prints the board of the sudokuManager it receives as an input.
 * Mode, addMarks parameters defines if errors will be marked, or not.
 */
void printSudokuGrid(struct sudokuManager *manager, enum Mode mode, int addMarks) {
    int n = manager->n, m = manager->m;
    int N = boardLen(manager) ;
    int i, j, k, p;

    for (i = 0; i < n; i++) { /* row of block matrix */
        printSeparatorRow(4*N + m + 1);
        for (p = 0; p < m; p++){ /* row inside block */
            for (j = 0; j < m; j++){ /* column of block matrix */
                printf("|");
                for (k = 0; k < n ; k++ ){ /* column inside block  */
                    printCell(manager, i*m + p, j*n + k, mode, addMarks);
                }
            }
            printf("|\n");
        }
    }
    printSeparatorRow(4*N + m + 1);
}

/* PARSER METHODS */

/*
 * This method receives a word and returns a number unique to the
 * command if it is a legal command, or -1 if it is an invalid command.
 * It returns the index of the command in the array commandList.
 */
int commandNum (char* word){
    int i;
    for (i = 0; i < commandListLength; i++){
        if (!strcmp(word, commandList[i])) { /* strings are equal*/
            return i;
        }
    }
    return -1;
}

/*
 * The function prints a message to the user saying
 * that not enough parameters were entered.
 */
void printFewParams(int numOfParams, int indexCommand){
    printf("Error: not enough parameters were entered.\n"
           "The number of parameters expected for the %s command is %d.\n", commandList[indexCommand], numOfParams);
}

/*
 * This function receives a command string,
 * and prints an error that notifies that too many parameters were entered.
 */
void printErrorTooManyParametersEntered(char* command){
    printf("Error: too many parameters were entered.\n"
           "The number of parameters expected for the %s command is ", command);
}

/*
 * The function prints a message to the user saying that
 * too many parameters were entered.
 * There are some possibilities for a valid number of parameters,
 * all of which are in the array arrNumOfParams.
 */
void printExtraParamsExtend(int *arrNumOfParams, int len, int indexCommand){
    int i;
    printErrorTooManyParametersEntered(commandList[indexCommand]);

    for (i = 0; i < len; i++){
        if (i == len - 1){
            if (i == 0){
                printf("%d.\n", arrNumOfParams[i]);
            }
            else{
                printf(" or %d.\n", arrNumOfParams[i]);
            }
        }
        else {
            if (i == 0) {
                printf("%d", arrNumOfParams[i]);
            }
            else{
                printf(", %d", arrNumOfParams[i]);
            }
        }
    }
}

/*
 * The function prints a message to the user saying that too many parameters were entered.
 * Assumes only one possibility for a valid number of parameters.
 */
void printExtraParams(int numOfParams, int indexCommand){
    int arrNumOfParams[1];
    arrNumOfParams[0] = numOfParams;
    printExtraParamsExtend(arrNumOfParams, 1, indexCommand);
}

/*
 * This method returns a string description of the mode of the game.
 */
char* modeToString(enum Mode mode){
    static char *strings[3] = {"Init", "Edit", "Solve"};
    return strings[mode];
}

/*
 * The function prints a message to the user saying that
 * the command is not available in the current mode.
 * It receives an array of modes in which the command is available and its length.
 */
void printUnavailableMode(int indexCommand, enum Mode mode, enum Mode *availableModes, int length){
    int i;
    printf("Error: the %s command is unavailable in %s mode.\n"
           "It is available in these modes: ", commandList[indexCommand], modeToString(mode));
    for (i = 0; i < length; i++){
        if (i == 0){
            printf("%s", modeToString(availableModes[i]));
        }
        else{
            printf(", %s", modeToString(availableModes[i]));
        }
    }
    printf("\n");
}

/*
 * This method prints a message to the user saying that
 * the value entered is not in the correct range.
 */
void printWrongRangeInt(int indexCommand, int value, int indexParam){
    printf("Error: the value %d entered is in the wrong range "
           "for parameter number %d of the %s command\n", value, indexParam, commandList[indexCommand]);
}

/*
 * This method prints a message to the user saying that
 * the value entered is not in the correct range.
 */
void printWrongRangeFloat(int indexCommand, float value, int indexParam){
    printf("Error: the value %f entered is in the wrong range"
           "for parameter number %d of the %s command\n", value, indexParam, commandList[indexCommand]);
}


/*
 * This method prints a message to the user saying that
 * the received parameter is not a number.
 */
void printNotANumber(int indexParam){
    printf("Error: an integer was expected for parameter %d, but was not received.\n", indexParam);
}

/*
 * This method prints a message to the user saying that
 * the received parameter is not a number.
 */
void printNotAFloat(int indexParam){
    printf("Error: a float was expected for parameter %d, but was not received.\n", indexParam);
}

/*
 * This method prints a message to the user saying that
 * the command entered does not exist.
 */
void printInvalidCommand(){
    printf("Error: the command entered does not exist. "
           "Please enter a new command.\n");
}

/*
 * This function prints an error message if the user calls "generate"
 * with X that is larger than the amount of empty cells in the board.
 * X > emptyCells.
 */
void printGenerateInputError(int emptyCells, int X){
    printf("Error: a request has been received to fill %d cells, while there are"
           " only %d empty cells.\n", emptyCells, X);
}

/* END OF PARSER METHODS*/

/* ERRORS */

/* MOVES LIST RELATED */

/*
 * This function announces that there is no more next moves,
 * can be called if the user tries to use "redo" function.
 */
void printNoNextMoveError(){
    printf("Error: there are no more next moves.\n");
}

/*
 * This method announces that there is no more next moves, can be called
 * if the user tries to "undo" of functions that uses it (e.g. "reset").
 */
void printNoPrevMoveError(){
    printf("Error: there are no more previous moves.\n");
}

/*
 * This function prints a change of a cell in the board.
 * Can be called from "undo" and "redo".
 * The format of cell print is <col, row>, similar to "set".
 */
void printActionWasMade(int row, int col, int prevVal, int newVal){
    printf("The value in cell <%d, %d> has"
           " been changed from %d to %d.\n", col+1, row+1, prevVal, newVal);
}

/* BOARD */

/*
 * This function prints a message that the board is erroneous.
 * Can be called when scanning an erroneous board from file (via: "solve"),
 * or when calling one of the following functions:
 * "autofill", "validate", "guess", "edit", and "guessHint".
 */
void printBoardIsErroneous(){
    printf("Error: the board is erroneous.\n");
}

/*
 * This function prints an error when the user tries
 * to change the value of a fixed cell.
 * Can be called from "set", "hint" and "guessHint".
 * The format of cell print is <col, row>, similar to "set".
 */
void printErrorCellXYIsFixed(int row, int col){
    printf("Error: cell <%d,%d> is fixed.\n", col + 1, row + 1); /* COL BEFORE ROW */
}

/*
 * This function prints an error when the user tries
 * to receive a hint for a cell that contains a value.
 * The format of cell print is <col, row>, similar to "set".
 */
void printErrorCellContainsValue(int row, int col){
    printf("Error: cell <%d,%d> already contains a value.\n", col + 1 , row + 1);
}

/* GUROBI RELATED PRINTS */

/*
 * This function is called if Gurobi process has failed.
 */
void printGurobiFailedTryAgain(){
    printf("Error: Gurobi has failed. Please try again.\n");
}

/*
 * This function is called if the user called "validate",
 * and the board turned out to be invalid.
 */
void printBoardNotValidError(){
    printf("The board is invalid.\n");
}

/*
 * This function is called if the user called "validate",
 * and the board turned out to be valid.
 */
void printBoardIsValid(){
    printf("The board is valid.\n");
}

/*
 * This function prints a message that function "generate" has failed
 * solving the board in all its attempts.
 */
void printGenerateFailed(){
    printf("Error: board generation failed after several attempts.\n");
}

/*
 * This function prints the amount of possible solutions of the board,
 * returned by "numSolutions".
 */
void printNumOfSolutions(int num){
    printf("There are %d solutions for the current board.\n", num);
}

/*
 * This function prints the possible values for a cell,
 * calculated by "guessHint" function for a single cell.
 * The format of cell print is <col, row>, similar to "set".
 */
void printValuesAndScores(int row, int col, int *array, double *scores, int length){
    int i;
    printf("The legal values for cell <%d,%d> "
           "and their scores are:\n", col + 1, row +1);
    printf("[");
    for (i = 0; i < length; i++){
        printf("(%d, %2f) ", array[i], scores[i]);
    }
    printf("]\n");
}

/*
 * This function prints the hint for the cell, as the user requested.
 * The format of cell print is <col, row>, similar to "set".
 */
void printHint(int row, int col, int hint){
    printf("Hint: set cell <%d,%d> to %d.\n", col + 1, row + 1, hint);
}

/* FILES RELATED */

/*
 * This function prints a message to the user if
 * the given file contains more characters than expected,
 * i.e. the format if the file is illegal.
 */
void printTooLongFile(){
    printf("Error: the file received is too long than expected.\n");
}

/*
 * This function prints an error to the user if the file path is illegal.
 */
void printFilePathIllegal(){
    printf("Error: file path is illegal.\n");
}

/*
 * This function prints a message that a number read from a file
 * is not in the correct range for the game.
 */
void printWrongRangeFile(int number, int start, int end){
    printf("Error: the number %d read from the file is not in the correct range."
           "The correct range is %d to %d.\n", number, start, end);
}

/*
 * This function prints a message that the input is not an integer.
 */
void printNotAnInteger(){
    printf("Error: input should be integer.\n");
}

/*
 * This function prints a message if an empty cell is set
 * as fixed by the addition of "." to the value in the file loaded.
 * The format of cell print is <col, row>, similar to "set".
 */
void printErrorEmptyCellFixed(int row, int col){
    printf("Error: cell <%d, %d> is empty but set as fixed.\n", col + 1, row + 1); /* COL is printed before ROW */
}

/*
 * This function prints that no input was received.
 */
void printNoInput(){
    printf("Error: no input was received.\n");
}

/*
 * This function prints that not enough numbers were entered.
 */
void printNotEnoughNumbers(){
    printf("Error: not enough numbers were inserted.\n");
}

/* MEMORY ALLOCATION FAILURE */

/*
 * This function prints an error that allocation failed~
 */
void printAllocFailed(){
    printf("Error: allocation failed.\n");
}


