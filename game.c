#include <stdio.h>
#include <stdlib.h>
#include "main_aux.h"
#include "game.h"
#include "Parser.h"
#include "utilitiesBoardManager.h"
#include "utilitiesLinkedList.h"

#define LENGTH 257

static enum Mode mode = Init;

/*
 * This method is used for printing the game's title.
 */
void title(){
    printGameTitle();
}
/*
 * This method changes the mode to the new mode
 */
void changeMode(enum Mode newMode){
    mode = newMode;
}

/* FILE HANDELING - LOAD AND SAVE TO FILES */

/*
 * This function attempts to receive an integer from file.
 * file is a valid pointer to an existing file.
 * If it fails, it returns 0, otherwise it returns 1.
 */
int inputNumFromFile(FILE *file, int *pNum){
    int check;
    check = fscanf(file, "%d", pNum);
    if (check != 1) {
        if (check == EOF) {
            printNoInput();
            return 0;
        }
        if (check == 0) {
            printNotAnInteger();
            return 0;
        }
    }
    return 1;
}


/*
 * This function loads a file and creates a sudoku board for it.
 * if the file format is illegal returns NULL
 */
struct sudokuManager* createBoardFromFile(char *fileName, enum Mode mode1){
    int n, m, i, j, success, value, *onlyFixed;
    FILE *file;
    struct movesList *linkedList;
    struct sudokuManager *board;
    char nextChar;

    linkedList = (struct movesList*)malloc(sizeof(struct movesList));
    if (linkedList == NULL) {
        printAllocFailed();
        return NULL;
    }

    board = (struct sudokuManager*)malloc(sizeof(struct sudokuManager));
    if (board == NULL) {
        free(linkedList);
        printAllocFailed();
        return NULL;
    }

    board->addMarks = 1;
    initList(linkedList);
    board->linkedList = linkedList;
    linkedList->board = board;

    /* board and linkedList are allocated */

    if((mode1 == Edit) && (fileName == NULL)) { /* we have been called by edit command and no fileName was received*/
        board->n = 3, board->m = 3;
    }
    else{ /* we need to read a board from a file*/
        file = fopen(fileName, "r");

        if (file == NULL) {
            printFilePathIllegal();
            free(board);
            free(linkedList);
            return NULL;
        }

        success = inputNumFromFile(file, &m);
        if(success == 0){ /*No integer was received*/
            free(board);
            free(linkedList);
            fclose(file);
            return NULL;
        }

        board->m = m;

        success = inputNumFromFile(file, &n);
        if(success == 0){ /*No integer was received*/
            free(board);
            free(linkedList);
            fclose(file);
            return NULL;
        }

        board->n = n;
    }

    board->fixed = (int *)calloc(boardArea(board), sizeof(int));
    if (board->fixed == NULL) {
        printAllocFailed();
        free(board);
        free(linkedList);
        if (fileName != NULL){
            fclose(file);
        }
        return NULL;
    }
    board->board = calloc(boardArea(board), sizeof(int));
    if (board->board == NULL) {
        printAllocFailed();
        free(board->fixed);
        free(board);
        free(linkedList);
        if (fileName != NULL){
            fclose(file);
        }
        return NULL;
    }

    board->erroneous= calloc(boardArea(board), sizeof(int));
    if (board->erroneous == NULL) {
        printAllocFailed();
        free(board->board);
        free(board->fixed);
        free(board);
        free(linkedList);
        if (file != NULL){ /* we opened a file*/
            fclose(file);
        }
        return NULL;
    }

    board->emptyCells = boardArea(board);

    /* all board's fields are initialized */

    if ((mode1 == Edit) && (fileName == NULL)){ /* we didn't receive a file path, we need to return board*/
        return board;
    }

    /* we need to read a board from a file*/

    for (i = 0; i < boardLen(board); i++) { /*Row*/
        for (j = 0; j < boardLen(board); j++) { /*Column*/
            success = inputNumFromFile(file, &value);
            if (success == 0) { /*No integer was received*/
                printNotEnoughNumbers(boardArea(board) + 2,i * boardLen(board) + j + 2);
                freeBoard(board); /*frees also linkedList */
                fclose(file);
                return NULL;
            }
            if (!isLegalCellValue(board, value)) { /* checking the cell is in the correct range */
                printWrongRange();
                freeBoard(board); /*frees also linkedList */
                fclose(file);
                return NULL;
            }
            changeCellValue(board->board, board->m, board->n, i, j, value);
            updateEmptyCellsSingleSet(board, 0, value);
            nextChar = fgetc(file);
            if (nextChar == EOF && (!isLastCellInMatrix(boardLen(board), i, j))) {
                printNotEnoughNumbers(boardArea(board), i * boardLen(board) + j);
                freeBoard(board); /*frees also linkedList */
                fclose(file);
                return NULL;
            }

            if ((nextChar == '.') && (mode1 == Solve)) {
                if (value != 0) {
                    changeCellValue(board->fixed, board->m, board->n, i, j, value);
                } else {
                    printErrorEmptyCellFixed(i + 1, j + 1);
                    freeBoard(board); /*frees also linkedList */
                    fclose(file);
                    return NULL;
                }
            }
        }


        if (mode1 == Solve) {

            onlyFixed = (int *) (calloc(boardArea(board), sizeof(int)));

            if (onlyFixed == NULL) {
                printAllocFailed();
                freeBoard(board); /*frees also linkedList */
                fclose(file);
                return NULL;
            }

            copyFixedOnly(board, onlyFixed);

            if (updateErroneousBoard(onlyFixed, board->erroneous, board->m, board->n)) { /* the board is erroneous */
                printErroneousBoard();
                freeBoard(board); /*frees also linkedList */
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    return board;
}

/*
 * This function uploads a file of a game to solve.
 * It returns -1 if we need to terminate. Otherwise, it returns 0.
 */
int solve(struct sudokuManager **pPrevBoard, char *fileName){
    struct sudokuManager *tmp;
    struct sudokuManager *board = (struct sudokuManager *) createBoardFromFile(fileName, Solve);
    if (board == NULL){ /* if board creation was unsuccessful */
        printErrorCreateBoard();
        return -1;
    }
    else{
        changeMode(Solve);
        tmp = *pPrevBoard;
        *pPrevBoard = board;
        if (tmp != NULL) {
            freeBoard(tmp);
        }
    }
    return 0;
}

/*
 * This function uploads a file of a game to edit.
 *  It returns -1 if we need to terminate. Otherwise, it returns 0.
 */
int edit(struct sudokuManager **pPrevBoard, char *fileName){
    struct sudokuManager *tmp;
    struct sudokuManager *board = (struct sudokuManager *) createBoardFromFile(fileName, Edit);
    if (board == NULL){ /* board creation was unsuccessful */
        printErrorCreateBoard();
        return -1;
    }
    else{
        changeMode(Edit);
        tmp = *pPrevBoard;
        *pPrevBoard = board;
        if (tmp != NULL) {
            freeBoard(tmp); /* frees the previous board if the new boars was successful */
        }
    }
    return 0;
}

/*
 * This method is used for saving your game.
 * need to validate the board!!!
 * Valid only in Edit and Solve modes.
 */
void save(struct sudokuManager *board, char* fileName){
    FILE *file;
    int N = boardLen(board), m=board->m, n=board->n, row, col, currVal;
    int valid = validateBoard(board);
    if(valid == 1){ /* the board is valid */
        file = fopen(fileName, "w");
        if(file == NULL){
            printFilePathIllegal();
            return;
        }
        fprintf(file, "%d %d \n", m, n);
        for (row = 0; row < N; row++) {
            for (col = 0; col < N; col++){
                currVal = board->board[matIndex(m, n, row, col)];
                if(((mode == Edit)||(isFixedCell(board, row, col))) && (currVal != 0)){
                    fprintf(file, "%d.", currVal);
                }
                else{
                    fprintf(file, "%d", currVal);
                }
                if(col != N-1){
                    fprintf(file, " ");
                }
                else if(row != N-1) {
                    fprintf(file,"\n");
                }
            }
        }
        fclose(file);
    } else if (valid == 0){
        printBoardNotValidError(); /* MUST SAVE ONLY VALID BOARD */
    }
    else if(valid == -1){
        return;
    }
    else{ /* valid == -2 - gurobi failed */
        printGurobiFailedTryAgain();
    }
}

/* MOVES RELATED FUNCTIONS */

/*
 * redo a move previously undone by the user.
 */
void redo(struct sudokuManager *board){
    if (board->linkedList->next == NULL){
        printNoNextMoveError();
    }
    else {
        redoCommand(board);
        printBoard(board);
    }
}

/*
 * undo a move previously done by the user.
 */
void undo(struct sudokuManager *board){
    if (board->linkedList->prev == NULL){
        printNoPrevMoveError();
    }
    else {
        undoCommand(board);
        printBoard(board);
    }
}

/*
 * This function resets the board to start position by undoing all moves.
*/
void reset(struct sudokuManager *board){
    pointToFirstMoveInMovesList(board);
    updateErroneousBoard(board->board, board->erroneous, board->m, board->n);
    printBoard(board);
}


/* BOARD CHANGING RELATED FUNCTIONS  */



/*
 * Sets in board at location [Y,X] ( X column, Y row ) value Z
 */
int set(struct sudokuManager *manager, int X, int Y, int Z){
    X--, Y--;
    if(isFixedCell(manager, Y, X)){
        printErrorCellXYIsFixed(X,Y);
        return 0;
    }
    else if ((doSet(manager, Y, X, Z) == -1) ||
            (createNextNode(manager, separator, 0, 0, 0, 0) == -1)) {
            printAllocFailed();
            return -1;
    }
    else {
        goToNextNode(manager);
        updateErroneousBoardCell(manager->board, manager->erroneous, manager->m, manager->n, Y, X);
        printBoard(manager);
        return 0;
     }
}


/*
 * This function automatically fill "obvious" values – cells which contain a single legal value.
 * Available only in Solve mode.
 */
int autofill(struct sudokuManager *board){
    if(isAnyErroneousCell(board)) {
        printBoardIsErroneous();
        return 0;
    }
    /* else... */
    if(updateAutofillValuesBoard(board)== -1){
        printAllocFailed();
        return -1;
    } /* no erroneous cells and no allocation failure.  */
    printBoard(board);
    return 0;
}

/* GUROBI RELATED FUNCTIONS */

/*
 * This function validates the current board using ILP,
 * ensuring it's solvable.
 * returns 1 if valid and 0 if it isn't .
 *  * CHECK!!!!!!!! DO WITH GUROBI
 */
int validate(struct sudokuManager *board){
    int isValid;
    if (isAnyErroneousCell(board)){
        printBoardIsErroneous();
        return 0;
    }

    isValid = validateBoard(board);
    if(isValid == 0){
        printBoardNotValidError();
        return 0;
    }
    else if(isValid == 1){
        printBoardIsValid();
        return 0;
    }
    else if(isValid == -2){
        printGurobiFailedTryAgain();
        return 0;
    }
    else{
        if (isValid == -1){
            printAllocFailed();
            return -1;
        }
    }
    return 0;
}


/*
 * This function shows the solution to cell <Y,X>
 * Valid only in Solve mode.
 * X = COLUMN, Y = ROW
 */
int hint(struct sudokuManager *board, int X, int Y){
    int hint = -3, ret;
    if(board->erroneous[matIndex(board->m, board->n, Y, X)]){
        printErrorCellIsErroneous(X, Y);
        return 0;
    }
    if(board->fixed[matIndex(board->m, board->n, Y, X)]){
        printErrorCellXYIsFixed(X, Y);
        return 0;
    }
    if(board->board[matIndex(board->m, board->n, Y, X)]!=0){
        printErrorCellContainsValue(X, Y);
        return 0;
    }
    ret = getHint(board, Y, X, &hint);
    if(ret == -1){
        return -1; /* alloc failed */
    }
    else
        if(ret == 0){
            printBoardNotValidError();
            return 0;
        }
        else{  /* ret == 1 */
        return hint; /* if this function returns -3 we have an error in getHint!!! */
    }

}


/*
 * This function guesses a solution to the current board using LP.
 * X value is between 0 and 1.
 * validation of X and mode is done in Parser.
 */
int guess(struct sudokuManager *board, float X){
    int res;
    if (isAnyErroneousCell(board)){
        printBoardIsErroneous();
        return 0;
    }

    res = doGuess(board, X);
    if (res == -1){
        printAllocFailed();
        return -1;
    }

    if (res == -2){
        printGurobiFailedTryAgain();
        return 0;
    }
    printf("%f", X);
    /* THINK OF A WAY TO DO IT MORE EFFICIENTLY IN GUROBI SECTION! */
    return 0;
}

/*
 * This function generates a board with Y random cells
 *  * CHECK!!!!!!!! check MALLOC ALLOCATION SUCCESS
 *  CREATE DOGENERATE IN GUROBI
 */
int generate(struct sudokuManager **pManager, int X, int Y){
    int *newBoard;
    struct sudokuManager *tmp, *prevManager;
    if (isAnyErroneousCell(*pManager)){
        printBoardIsErroneous();
        return 0;
    }
    newBoard = (int*)calloc(boardArea(*pManager), sizeof(int));
    if(newBoard == NULL){
        printAllocFailed();
        return -1;
    } /* MAYBE DONE IN PARSER */
    if(X > amountOfEmptyCells(*pManager)){
        printGenerateInputError();
        return 0;
    }
    else {
        tmp = doGenerate(*pManager, newBoard, X, Y);
        if(tmp == NULL){ /* we need to terminate */
            printAllocFailed();
            free(newBoard);
            return -1;
        }
        prevManager = *pManager;
        *pManager = tmp;
        freeBoard(prevManager);
    }
    return 0;
}



/*
 * This method is used for exiting the game.
 * It returns 2.
 */
int exitGame(struct sudokuManager *board){
    printExitMessage();
    freeBoard(board);
    return 2;
}

/* PRINT RELATED FUNCTIONS */

void printBoard(struct sudokuManager *board){
    printSudokuGrid(board, mode);
    /* NEED TO CHECK THE PRINTING FORMAT */
}


/*
 * This function sets mark error to X
 * X and mode are being checked in Parser
 */
void markErrors(struct sudokuManager* board ,int X){
    board->addMarks = X;
}


/*
 * This function shows the solution to cell <Y,X>
 * Valid only in Solve mode.
 */
int guessHint(struct sudokuManager *board, int X, int Y){
    int length, *cellValues = NULL, res;
    X--, Y--;
    if (isAnyErroneousCell(board)){
        printBoardIsErroneous();
        return 0;
    }
    if (isFixedCell(board, Y, X)){ /* cell is fixed */
        printErrorCellXYIsFixed(X, Y);
        return 0;
    }
    if (board->board[matIndex(board->m, board->n, Y, X)] != 0){
        printErrorCellIsSet();
        return 0;
    }

    res = doGuessHint(board, Y, X, &cellValues, &length);

    if (res == -1){
        return 0;
    }

    if (res == -2){
        return -1;
    }

    if (res == 0){ /* we have succeeded and we need to free cellValues */
        printGuessHintSuccess();
        printArray(cellValues, length);
        free(cellValues);
        return 0;
    }

    return 0;
}


/*
 * This method starts a game.
 * This method performs a whole game. It initializes the  board,
 * receives commands from the user and executes them.
 * It returns -1 when we need to terminate.
 */
int startGame(){
    char command[LENGTH];  /*Last cell is '\0'*/
    int res;
    struct sudokuManager *board = NULL;

    title();
    while (fgets(command, LENGTH, stdin) != NULL){  /* We have not reached EOF*/
        if (command[0] == '\n'){ /* Line is empty. */
            continue;
        }
        /*The line is not blank*/
        res = interpret(command, &board, mode);
        if (res == -1){ /* There is any error */
            return -1;
        }
        if (res == 2) { /* exit command was entered*/
            return 0;
        }
    }
    /* We have reached EOF, there are no more commands to execute*/
    return exitGame(board);
}

