#include <stdio.h>
#include "main_aux.h"

#include "game.h"
#define title1 "                              ,,                               \n .M\"\"\"bgd                   `7MM              `7MM                  \n,MI    \"Y                     MM                MM                  \n`MMb."
#define title2    "     `7MM  `7MM     ,M\"\"bMM    ,pW\"Wq.     MM  ,MP'  `7MM  `7MM  \n  `YMMNq.   MM    MM   ,AP    MM   6W'   `Wb    MM ;Y       MM    MM  \n.     `MM   MM    MM   8MI    MM   8M     M8    MM;Mm       MM    MM  \nMb     dM   MM    MM   `Mb    MM   YA.   ,A9    MM `Mb.     MM    MM  \nP\"Ybmmd\"    `Mbod\"YML.  `Wbmd\"MML.  `Ybmd9'.   JMML. YA.    `Mbod\"YML."
#define creators "Din Moshe and Doron Antebi"


static char* commandList[] = {"solve", "edit", "mark_errors", "print_board", "set", "validate",
                              "guess", "generate", "undo", "redo", "save", "hint", "guess_hint",
                              "num_solutions", "autofill", "reset", "exit"};

void printGameTitle(){
    printf("%s%s\n\n             Creators: %s.\n", title1, title2, creators);
}

/* PARSER METHODS */

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
           "The number of parameters expected for the %s command is ", commandList[indexCommand]);
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
    printf("There are not enough empty cells. There are %d empty cells, "
           "but %d empty cells are required\n", emptyCells, requiredEmptyCells);
}


/* END OF PARSER METHODS*/



void printNoNextMoveError(){
    printf("There is no next move\n");
}

void printErrorAutofill(){
    printf("Autofill error!\n");
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
    printf("No input was received.\n");
}

/*
 * This method prints that not enough numbers were entered.
 */
void printNotEnoughNumbers(int expected, int is){
    printf("Not enough numbers were inserted. Expected %d, but received only %d.\n", expected, is);
}

/*
 * This method prints a message saying that an empty cell is set as fixed in the file loaded.
 */
void printErrorEmptyCellFixed(int row, int col){
    printf("Cell (%d, %d) is empty but set as fixed.\n", row, col);
}

void printBoardNotValidError(){
    printf("The board is invalid!! \n");
}

/*
 * This function prints an error that allocation failed~
 */
void printAllocFailed(){
    printf("Allocation failed!! \n");
}

/*
 * This method prints a message that the board is erronous
 */
void printBoardIsErroneous(){
    printf("The board is erroneous!\n");
}

/*
 * This function prints error that
 * ((value < 0) || (x > boardLen(manager)));
 */
void printWrongRange(struct sudokuManager *manager, int value){
    if(value < 0){
        printf("%d < 0, ", value);
    }
    else { /* value > boardLen(mmanager)*/
        printf("%d > %d, ", value, boardLen(manager));
    }
    printf("while value should be in range 0 to %d.\n", boardLen(manager));
}


/*
 * This function prints a message that the input is not an integer.
 */
void printNotAnInteger(){
    printf("Input should be integer.\n");
}


