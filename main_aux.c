#include <stdio.h>
#include "main_aux.h"

#include "game.h"
#include <string.h>
#define title1 "                              ,,                               \n .M\"\"\"bgd                   `7MM              `7MM                  \n,MI    \"Y                     MM                MM                  \n`MMb."
#define title2    "     `7MM  `7MM     ,M\"\"bMM    ,pW\"Wq.     MM  ,MP'  `7MM  `7MM  \n  `YMMNq.   MM    MM   ,AP    MM   6W'   `Wb    MM ;Y       MM    MM  \n.     `MM   MM    MM   8MI    MM   8M     M8    MM;Mm       MM    MM  \nMb     dM   MM    MM   `Mb    MM   YA.   ,A9    MM `Mb.     MM    MM  \nP\"Ybmmd\"    `Mbod\"YML.  `Wbmd\"MML.  `Ybmd9'.   JMML. YA.    `Mbod\"YML."
#define creators "Din Moshe and Doron Antebi"

#define commandListLength 17
static char* commandList[] = {"solve", "edit", "mark_errors", "print_board", "set", "validate",
                              "guess", "generate", "undo", "redo", "save", "hint", "guess_hint",
                              "num_solutions", "autofill", "reset", "exit"};

void printGameTitle(){
    printf("%s%s\n\n             Creators: %s.\n", title1, title2, creators);
}

/*
 * This function prints the exit message.
 */
void printExitMessage(){
    printf("Bye bye! (:\n");
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
 * The function prints a message to the user saying that not enough parameters were entered.
 */
void printFewParams(int numOfParams, int indexCommand){
    printf("Error: not enough parameters were entered.\n"
           "The number of parameters expected for the %s command is %d.\n", commandList[indexCommand], numOfParams);
}

/*
 * The function prints a message to the user saying that too many parameters were entered.
 * There are some possibilities for a valid number of parameters, all of which are in the array arrNumOfParams.
 */
void printExtraParamsExtend(int *arrNumOfParams, int len, int indexCommand){
    int i;

    printf("Error: too many parameters were entered.\n"
           "The number of parameters expected for the %s command is .\n", commandList[indexCommand]);
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
 * This method returns a string description of mode.
 */
char* modeToString(enum Mode mode){
    static char *strings[3] = {"Init", "Edit", "Solve"};
    return strings[mode];
}

/*
 * The function prints a message to the user saying that the command is not available in the current mode.
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
 * This method prints a message to the user saying that the value entered is not in the correct range.
 */
void printWrongRangeInt(int indexCommand, int value, int indexParam){
    printf("Error: the value %d entered is in the wrong range"
           "for parameter number %d of the %s command\n", value, indexParam, commandList[indexCommand]);
}

/*
 * This method prints a message to the user saying that the value entered is not in the correct range.
 */
void printWrongRangeFloat(int indexCommand, float value, int indexParam){
    printf("Error: the value %f entered is in the wrong range"
           "for parameter number %d of the %s command\n", value, indexParam, commandList[indexCommand]);
}


/*
 * This method prints a message to the user saying that the received parameter is not a number.
 */
void printNotANumber(int indexParam){
    printf("Error: an integer was expected for parameter %d, but was not received.\n", indexParam);
}

/*
 * This method prints a message to the user saying that the received parameter is not a number.
 */
void printNotAFloat(int indexParam){
    printf("Error: a float was expected for parameter %d, but was not received.\n", indexParam);
}

/*
 * This method prints a message to the user saying that there are not enough empty cells.
 */
void printNotEnoughEmptyCells(int emptyCells, int requiredEmptyCells){
    printf("Error: there are not enough empty cells. There are %d empty cells, "
           "but %d empty cells are required\n", emptyCells, requiredEmptyCells);
}

/*
 * This method prints a message to the user saying that the command entered does not exist.
 */
void printInvalidCommand(){
    printf("Error: the command entered does not exist. Please enter a new command.\n");
}

/* END OF PARSER METHODS*/



void printNoNextMoveError(){
    printf("Error: there are no more next moves.\n");
}

void printNoPrevMoveError(){
    printf("Error: there are no more previous moves.\n");
}

/* PRINT RELATED METHODS */
/*
 * length should be equal to 4N+m+1
 */
void printSeparatorRow(int length){
    int i;
    for (i = 0; i < length ; i++) {
        printf("-");
    }
    printf("\n");
}

/*
 *
 */
void printCellRow(){
    printf("|");
}

/*
 * This method prints the cell (row, col)
 */
void printCell(struct sudokuManager *manager, int row, int col, enum Mode mode){
    int* puzzle = manager->board;
    int addErrors = 0;
    int index;
        if (manager->addMarks == 1 || mode == Edit){
        addErrors = 1;
    }
    index = matIndex(manager->m, manager->n, row, col);
    if(isFixedCell(manager, row, col)){
        printf("%2d.",puzzle[index]);
        return;
    }
    if(isErroneous(manager, row, col) && addErrors){
        printf("%2d*",puzzle[index]);
    } else {
        if(puzzle[index] == 0){
            printf("  ");
        } else{
            printf("%2d",puzzle[index]);
        }
    }

}

void printSudokuGrid(struct sudokuManager *manager, enum Mode mode) {
    int n = manager->n, m = manager->m;
    int N = boardLen(manager) ;
    int i, j, k;
    for (i = 0; i < m; i++) {
        printSeparatorRow(4*N + m + 1);
        for (j = 0; j < n; j++){
            printCellRow();
            for (k = 0; k < m ; k++ ){
                printCell(manager, i*n + j, j*m + k, mode);
            }
            printSeparatorRow(4*N + m + 1);
        }
    }
}

/*
 * This method prints that no input was received.
 */
void printNoInput(){
    printf("Error: no input was received.\n");
}

/*
 * This method prints that not enough numbers were entered.
 */
void printNotEnoughNumbers(){
    printf("Error: not enough numbers were inserted.\n");
}

/*
 * This method prints a message saying that an empty cell is set as fixed in the file loaded.
 */
void printErrorEmptyCellFixed(int row, int col){
    printf("Error: cell <%d, %d> is empty but set as fixed.\n", col, row); /* COL is printed before ROW */
}


/*
 * This function prints an error if the path for a file, given by user, is illegal.
 */
void printFilePathIllegal(){
    printf("Error: file path is illegal.\n");


}
/*
 * This method prints that the number read from a file is not in the correct range.
 */
void printWrongRangeFile(int number, int start, int end){
    printf("Error: the number %d read from the file is not in the correct range."
           "The correct range is %d to %d.\n", number, start, end);
}

/* */
void printGurobiFailedTryAgain(){
    printf("Error: Gurobi has failed. Please try again.\n");
}

void printBoardNotValidError(){
    printf("Error: the board is invalid.\n");
}

/*
 * This function prints an error that allocation failed~
 */
void printAllocFailed(){
    printf("Error: allocation failed.\n");
}

/*
 * This method prints a message that the board is erronous
 */
void printBoardIsErroneous(){
    printf("Error: the board is erroneous.\n");
}

/*
 * This method announces that the user has been trying to set a fixed cell
 * X is column, Y is row.
 */
void printErrorCellXYIsFixed(int col, int row){
    printf("Error: cell <%d,%d> is fixed.\n", col, row); /* COL BEFORE ROW */
}

/*
 * This message is printed when trying to call hint with an erroneous cell
 */
void printErrorCellIsErroneous(int col, int row){
    printf("Error: cell <%d,%d> is erroneous.\n", col, row);
}

/*
 * This message is printed when trying to call hint with a cell that contains a value
 */
void printErrorCellContainsValue(int col, int row){
    printf("Error: cell <%d,%d> already contains a value.\n", col, row);
}

/*
 * This function prints a message that the input is not an integer.
 */
void printNotAnInteger(){
    printf("Error: input should be integer.\n");
}

/*
 *
 */
void printBoardIsValid(){
    printf("The board is valid.\n");
}


/*
 * Error message if X>emptyCells.
 */
void printGenerateInputError(int emptyCells, int X){
    printf("Error: a request has been received to fill %d cells, while there are only %d empty cells.\n", emptyCells, X);
}

void printBoardIsSolved(){
    printf("Congratulations! The board is successfully solved!\n");
}

/*
 * This method prints a message to the user saying how many possible values there are.
 */
void printNumOfSolutions(int num){
    printf("There are %d solutions sfor the current board.\n", num);
}


/*
 * prints the array of possible legal values for guess hint
 */
void printArray(int *array, int length){
    int i;
    printf("[");
    for (i = 0; i < length; i++){
        printf("%d ", array[i]);
    }
    printf("]\n");
}

