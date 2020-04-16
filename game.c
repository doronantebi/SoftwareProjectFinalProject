#include <stdio.h>
#include <stdlib.h>
#include "main_aux.h"
#include "game.h"
#include "Parser.h"
#include "utilitiesBoardManager.h"
#include "utilitiesLinkedList.h"

static enum Mode mode = Init;
static int addMarks = 1;

/*
 * This method changes the mode of the game into newMode.
 * Mode is one of the following: Init, Edit and Solve.
 */
void changeMode(enum Mode newMode){
    mode = newMode;
}

/*
 * This function changes the value of addMarks parameter to X
 * if (addMark == 1) printBoard method will mark erroneous cells.
 */
void markErrors(int X){
    addMarks = X;
}

/* FILE HANDELING - LOAD AND SAVE TO FILES */

/*
 * This function attempts to receive an integer from file.
 * If it fails, it returns 0, otherwise it returns 1.
 * file: a valid pointer to an existing file.
 * pNum: a pointer to integer, in which we set the integer received from file.
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
 * if the file format is illegal returns -2, if memory allocation failed, it returns -1.
 * If no memory allocation error occurs and file contains a legal format of a board, it returns 0.
 */
int createBoardFromFile(char *fileName, enum Mode mode1, struct sudokuManager *board){
    int n, m, i, j, success, value, *onlyFixed;
    FILE *file = NULL;
    struct movesList *linkedList;
    char nextChar;

    linkedList = (struct movesList*)malloc(sizeof(struct movesList));
    if (linkedList == NULL) {
        return -1;
    }

    initList(linkedList);
    board->linkedList = linkedList;
    linkedList->board = board;

    /* linkedList is allocated */

    if((mode1 == Edit) && (fileName == NULL)) { /* we have been called by edit command and no fileName was received*/
        board->n = 3, board->m = 3;
    }
    else{ /* we need to read a board from a file*/
        file = fopen(fileName, "r");

        if (file == NULL) {
            printFilePathIllegal();
            return -2;
        }

        success = inputNumFromFile(file, &m);
        if(success == 0){ /*No integer was received*/
            fclose(file);
            return -2;
        }

        board->m = m;

        success = inputNumFromFile(file, &n);
        if(success == 0){ /*No integer was received*/
            fclose(file);
            return -2;
        }

        board->n = n;
    }

    board->fixed = (int *)calloc(boardArea(board), sizeof(int));
    if (board->fixed == NULL) {
        if (file != NULL){
            fclose(file);
        }
        return -1;
    }
    board->board = (int *)calloc(boardArea(board), sizeof(int));
    if (board->board == NULL) {
        if (file != NULL){
            fclose(file);
        }
        return -1;
    }

    board->erroneous= calloc(boardArea(board), sizeof(int));
    if (board->erroneous == NULL) {
        if (file != NULL){ /* we opened a file*/
            fclose(file);
        }
        return -1;
    }

    board->emptyCells = boardArea(board);

    /* all board's fields are initialized */

    if ((mode1 == Edit) && (fileName == NULL)){ /* we didn't receive a file path, we need to return board*/
        return 0;
    }

    /* we need to read a board from a file*/

    for (i = 0; i < boardLen(board); i++) { /*Row*/
        for (j = 0; j < boardLen(board); j++) { /*Column*/
            success = inputNumFromFile(file, &value);
            if (success == 0) { /*No integer was received*/
                printNotEnoughNumbers();
                fclose(file);
                return -2;
            }
            if (!isLegalCellValue(board, value)) { /* checking the cell is in the correct range */
                printWrongRangeFile(value, 1, boardLen(board));
                fclose(file);
                return -2;
            }
            changeCellValue(board->board, board->m, board->n, i, j, value);
            updateEmptyCellsSingleSet(board, 0, value);
            nextChar = fgetc(file);
            if (nextChar == EOF && (!isLastCellInMatrix(boardLen(board), i, j))) {
                printNotEnoughNumbers();
                fclose(file);
                return -2;
            }

            if ((nextChar == '.') && (mode1 == Solve)) {
                if (value != 0) {
                    changeCellValue(board->fixed, board->m, board->n, i, j, value);
                } else {
                    printErrorEmptyCellFixed(i + 1, j + 1);
                    fclose(file);
                    return -2;
                }
            }
        }
    }

    if (fscanf(file, "%c", &nextChar) != EOF){
        printTooLongFile();
        fclose(file);
        return -2;
    }

    if (mode1 == Solve) {

        onlyFixed = (int *) (calloc(boardArea(board), sizeof(int)));

        if (onlyFixed == NULL) {
            fclose(file);
            return -1;
        }

        copyFixedOnly(board, onlyFixed);

        if (updateErroneousBoard(onlyFixed, board->erroneous, board->m, board->n)) { /* the board is erroneous */
            printBoardIsErroneous();
            free(onlyFixed);
            fclose(file);
            return -2;
        }
        free(onlyFixed);
    }


    fclose(file);
    return 0;
}

/*
 * This function uploads a file of a game to be used in mode1.
 *  It returns -1 if we need to terminate. Otherwise, it returns 0.
 *  If it fails, the current mode of the game will not change into mode1,
 *  and the current board will be the previous board that the user played.
 */
int loadFile(struct sudokuManager **pPrevBoard, char *fileName, enum Mode mode1){
    struct sudokuManager *tmp;
    struct sudokuManager *board;
    int res;
    board = (struct sudokuManager*)malloc(sizeof(struct sudokuManager));
    if (board == NULL) {
        printAllocFailed();
        return -1;
    }
    initNullBoard(board);

    res = createBoardFromFile(fileName, mode1, board);
    if (res == -1){ /* if board creation was unsuccessful */
        printAllocFailed();
        freeBoard(board); /* frees also linked list */
        return -1;
    }
    else{
        if (res == -2){ /* file format was illegal */
            freeBoard(board);
            return 0;
        }
        else{
            changeMode(mode1);
            tmp = *pPrevBoard;
            *pPrevBoard = board;
            if (tmp != NULL) {
                freeBoard(tmp);
            }
            printBoard(board);
        }
    }
    return 0;
}

/*
 * This function uploads a file of a game to solve.
 * It returns -1 if we need to terminate.
 * Otherwise, it returns 0.
 */
int solve(struct sudokuManager **pPrevBoard, char *fileName) {
    return loadFile(pPrevBoard, fileName, Solve);
}

/*
 * This function uploads a file of a game to edit.
 *  It returns -1 if we need to terminate.
 *  Otherwise, it returns 0.
 */
int edit(struct sudokuManager **pPrevBoard, char *fileName){
    return loadFile(pPrevBoard, fileName, Edit);
}

/*
 * This function is used for saving a game into the file the user set   .
 * This game can be uploaded by its file with "edit" and "solve" commands.
 * Invalid board can not be saved in Edit mode.
 * If an allocation failed during validation, the program terminates.
 * If a ILP error cause the validation to fail, it prints a relevant message.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int save(struct sudokuManager *board, char* fileName){
    FILE *file;
    int valid;
    int N = boardLen(board), m=board->m, n=board->n, row, col, currVal;
    if((mode == Solve)||((valid = validateBoard(board)) == 1)){ /* the board is valid or the mode == Solve*/
        file = fopen(fileName, "w");
        if(file == NULL){
            printFilePathIllegal();
            return 0;
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
        printAllocFailed();
        return -1;
    }
    else{
        if (valid == -2)/* gurobi failed */
        printGurobiFailedTryAgain();
    }
    return 0;
}

/* MOVES RELATED FUNCTIONS */

/*
 * This function undoes a move previously done by the user.
 * If there is no move to undo (it's the board initial state) it prints an error message.
 * Each change that has been made in that board will be printed in a message.
 */
void undo(struct sudokuManager *board){
    int res, row, col;
    if(board->linkedList == NULL){
        printf("Error: linked list is NULL(in undo).\n");
    }
    if (board->linkedList->prev == NULL){
        printNoPrevMoveError();
    }
    else {
        res = undoCommand(board, 1);
        if (res > 1){
            updateErroneousBoard(board->board, board->erroneous, board->m, board->n);
        }
        else{
            if (res == 1){
                row = board->linkedList->next->row;
                col = board->linkedList->next->col;
                updateErroneousBoardCell(board->board, board->erroneous, board->m, board->n, row, col);
            }
        }
        printBoard(board);
    }
}


/*
 * This function redoes a move previously undone by the user.
 * If there is no move to redo it prints an error message.
 * Each change that has been made in that board will be printed in a message.
 */
void redo(struct sudokuManager *board){
    int res, row, col;
    if(board->linkedList == NULL){
        printf("Error: linked list is NULL(in redo).\n");
    }
    if (board->linkedList->next == NULL){
        printNoNextMoveError();
    }
    else {
        res = redoCommand(board, 1);
        if (res > 1){
            updateErroneousBoard(board->board, board->erroneous, board->m, board->n);
        }
        else{
            if (res == 1){
                row = board->linkedList->prev->row;
                col = board->linkedList->prev->col;
                updateErroneousBoardCell(board->board, board->erroneous, board->m, board->n, row, col);
            }
        }
        printBoard(board);
    }
}

/*
 * This function resets the board to its initial state by undoing all moves.
 * This function will not change the moves list, but moving the pointer to its beginning.
*/
void reset(struct sudokuManager *board){
    pointToFirstMoveInMovesList(board, 0);
    updateErroneousBoard(board->board, board->erroneous, board->m, board->n);
    printBoard(board);
}

/* BOARD CHANGING RELATED FUNCTIONS  */

/*
 * This function sets val into the board in cell <col, row>.
 * Will not allow setting a value into a fixed cell.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int set(struct sudokuManager *manager, int col, int row, int val){
    col--, row--;
    if(isFixedCell(manager, row, col)){
        printErrorCellXYIsFixed(row, col);
        return 0;
    }
    else if ((doSet(manager, row, col, val) == -1) ||
             (createNextNode(manager, separator, 0, 0, 0, 0) == -1)) {
        printAllocFailed();
        return -1;
    }
    else {
        goToNextNode(manager);
        updateErroneousBoardCell(manager->board, manager->erroneous,
                                manager->m, manager->n, row, col);
        printBoard(manager);
        return 0;
    }
}


/*
 * This function automatically fill "obvious" values: cells which contain only a single legal value.
 * This function will print an error when used on erroneous board.
 * It returns -1 if we need to terminate, and 0 otherwise.
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
 * This function validates the current board using ILP, ensuring it's solvable.
 * Either way, it prints the relevant message.
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
 * This function shows a possible solution to cell <col, row> using ILP.
 * If the board has several solutions, a hint for a single cell might change.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int hint(struct sudokuManager *board, int col, int row){
    int hint, ret;
    row--, col--;
    if(isAnyErroneousCell(board)){
        printBoardIsErroneous();
        return 0;
    }
    if(board->fixed[matIndex(board->m, board->n, row, col)]){
        printErrorCellXYIsFixed(row, col);
        return 0;
    }
    if(board->board[matIndex(board->m, board->n, row, col)] != 0){
        printErrorCellContainsValue(row, col);
        return 0;
    }
    ret = getHint(board, row, col, &hint);
    if(ret == -1){
        printAllocFailed();
        return -1; /* allocation failed */
    }
    else
    if(ret == 0){
        printBoardNotValidError();
        return 0;
    }
    else{  /* ret == 1 */
        printHint(row, col, hint);
        return 0;
    }
}

/*
 * This function guesses a solution to the current board using LP.
 * It fills cells with probability higher than X to appear in a valid solution.
 * If The board is erroneous, prints an error message.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int guess(struct sudokuManager *board, float X){
    int res, *retBoard;
    if (isAnyErroneousCell(board)){
        printBoardIsErroneous();
        return 0;
    }
    retBoard = (int *)calloc(boardArea(board), sizeof(int));
    if (retBoard == NULL){
        printAllocFailed();
        return -1;
    }

    res = doGuess(board, X, retBoard);
    if (res == -1){
        printAllocFailed();
        free(retBoard);
        return -1;
    }

    if (res == -2){
        printGurobiFailedTryAgain();
        free(retBoard);
        return 0;
    }
    if (res == 0){
        printBoardNotValidError();
        free(retBoard);
    }
    if (res == 1){
        if (updateBoardLinkedList(board, retBoard) == -1){
            printAllocFailed();
            free(retBoard);
            return -1;
        }
        free(retBoard);
        printBoard(board);
    }

    return 0;
}

/*
 * This function generates a board from the current board by filling X random cells,
 * solving the filled board, and leaving Y cells filled.
 * If Generate fails the board will remain the previous board.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int generate(struct sudokuManager **pManager, int X, int Y){
    int *retBoard; /* THIS WILL CONTAIN THE SOLUTION */
    int res;

    if (isAnyErroneousCell(*pManager)){
        printBoardIsErroneous();
        return 0;
    }
    retBoard = (int*)calloc(boardArea(*pManager), sizeof(int));
    if(retBoard == NULL){
        printAllocFailed();
        return -1;
    }

    res = doGenerate(*pManager, X, Y, retBoard);
    if(res == -1){ /* we need to terminate */
        printAllocFailed();
        free(retBoard);
        return -1;
    }
    if (res == 1){
        if(updateBoardLinkedList(*pManager, retBoard) == -1) {
            printAllocFailed();
            free(retBoard);
            return -1;
        }
        printBoard(*pManager);
        free(retBoard);
    }
    if (res == 0){
        printGenerateFailed();
    }

    return 0;
}



/*
 * This function terminates the game, and frees used resources.
 * It returns 2.
 */
int exitGame(struct sudokuManager *board){
    printExitMessage();
    if (board != NULL){
        freeBoard(board);
    }
    return 2;
}

/* PRINT RELATED FUNCTIONS */

/*
 * This function prints the board.
 * If the board is filled, and the mode is solved,
 * it return if the board is erroneous or not.
 * If the board is legally filled, will announce the user that the board is successfully solved,
 * and change mode to Init.
 */
void printBoard(struct sudokuManager *board){
    printSudokuGrid(board, mode, addMarks);
    if (mode == Solve && board->emptyCells == 0){
        if (isAnyErroneousCell(board)){
            printBoardIsErroneous();
        }
        else{
            printBoardIsSolved();
            changeMode(Init);
        }
    }
}

/*
 * This function shows the solution to cell <row,col>,
 * and prints the scored (probabilities) for each value.
 * This function prints an error if the user tries to execute it when one of the following:
 * The board is erroneous, the cell is fixed, or the cell is filled with a value.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int guessHint(struct sudokuManager *board, int col, int row){
    int length, *cellValues = NULL, res;
    double *scores;
    col--, row--;
    if (isAnyErroneousCell(board)){
        printBoardIsErroneous();
        return 0;
    }
    if (isFixedCell(board, row, col)){ /* cell is fixed */
        printErrorCellXYIsFixed(row, col);
        return 0;
    }
    if (board->board[matIndex(board->m, board->n, row, col)] != 0){
        printErrorCellContainsValue(row, col);
        return 0;
    }

    res = doGuessHint(board, row, col, &cellValues, &scores, &length);

    if (res == -2){
        printGurobiFailedTryAgain();
        return 0;
    }

    if (res == -1){
        printAllocFailed();
        return -1;
    }

    if (res == 1){ /* we have succeeded and we need to free cellValues and scores */
        printValuesAndScores(row, col, cellValues, scores, length);
        free(cellValues);
        free(scores);
        return 0;
    }
    printBoardNotValidError();
    return 0;
}

/*
 * This function prints the amount of possible solutions of the board,
 * by using exhaustive backtracking.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int numSolutions(struct sudokuManager *board){
    int res;

    if (isAnyErroneousCell(board)){
        printBoardIsErroneous();
        return 0;
    }
    /* board is not erroneous */
    if (board->emptyCells == 0){ /* board is solved */
        printNumOfSolutions(1);
        printBoardIsFull();
        return 0;
    }

    res = backtracking(board);
    if (res == -1){
        printAllocFailed();
        return -1;
    }
    else{
        printNumOfSolutions(res);
        return 0;
    }
}

/* START GAME */

/*
 * This function is used to starts a game,
 * receives commands from the user and executes them.
 * It returns -1 when we need to terminate.
 */
int startGame(){
    char command[LENGTH];  /*Last cell is '\0'*/
    int res;
    struct sudokuManager *board = NULL;

    printGameTitle();
    while (fgets(command, LENGTH, stdin) != NULL){  /* We have not reached EOF*/
        if (command[0] == '\n'){ /* Line is empty. */
            continue;
        }
        /*The line is not blank*/
        res = interpret(command, &board, mode);
        if (res == -1){ /* There is any error */
            if (board != NULL){
                freeBoard(board);
            }
            return -1;
        }
        if (res == 2) { /* exit command was entered*/
            return 0;
        }
    }
    /* We have reached EOF, there are no more commands to execute*/
    return exitGame(board);
}

