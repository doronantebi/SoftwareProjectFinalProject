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
    board->linkedList = linkedList;
    initList(linkedList);
    linkedList->board = board;

    if((mode1 == Edit) && (fileName == NULL)){
        board->n = 3, board->m = 3; board->linkedList = linkedList;
        board->fixed = calloc(9 * 9, sizeof(int));
        if (board->fixed == NULL){
            printAllocFailed();
            free(board);
            free(linkedList);
            return NULL;
        }
        board->board = calloc(9 * 9, sizeof(int));
        if (board->board == NULL){
            printAllocFailed();
            free(board);
            free(linkedList);
            return NULL;
        }
    }
    else {
        file = fopen(fileName, "r");
        if (file == NULL) {
            printFilePathIllegal();
            freeBoard(board);
            free(linkedList);
            return NULL;
        }
        success = inputNumFromFile(file, &m);
        if(success == 0){ /*No integer was received*/
            freeBoard(board);
            free(linkedList);
            fclose(file);
            return NULL;
        }
        board->m = m;

        success = inputNumFromFile(file, &n);
        if(success == 0){ /*No integer was received*/
            freeBoard(board);
            free(linkedList);
            fclose(file);
            return NULL;
        }
        board->n = n;

        board->board = (int *)(calloc(sizeof(int), boardArea(board)));

        if (board->board == NULL){
            printAllocFailed();
            freeBoard(board);
            free(linkedList);
            fclose(file);
            return NULL;
        }

        for (i = 0; i < boardLen(board); i++){ /*Row*/
            for (j = 0; j < boardLen(board); j++){ /*Column*/
                success = inputNumFromFile(file, &value);
                if(success == 0){ /*No integer was received*/
                    freeBoard(board);
                    free(linkedList);
                    fclose(file);
                    return NULL;
                }
                if (!isLegalCellValue(board, value)){
                    printWrongRange(board, value);
                    freeBoard(board);
                    free(linkedList);
                    fclose(file);
                    return NULL;
                }
                changeCellValue(board->board, board->m, board->n, i, j, value);
                nextChar = fgetc(file);
                if (nextChar == EOF){
                    printNotEnoughNumbers(boardArea(board), i*boardLen(board) + j);
                    freeBoard(board);
                    free(linkedList);
                    fclose(file);
                    return NULL;
                }

                if ((nextChar == '.') && (mode1==Solve)){
                    if (value != 0){
                        changeCellValue(board->fixed, board->m, board->n, i, j, value);
                    }
                    else{
                        printErrorEmptyCellFixed(i+1, j+1);
                        freeBoard(board);
                        free(linkedList);
                        fclose(file);
                        return NULL;
                    }
                }
            }
        }

        if (mode1 == Solve){
            onlyFixed = (int *) (calloc(sizeof(int), boardArea(board)));

            if (onlyFixed == NULL){
                printAllocFailed();
                freeBoard(board);
                free(linkedList);
                fclose(file);
                return NULL;
            }

            copyFixedOnly(board, onlyFixed);

            if (updateErroneousBoard(onlyFixed, board->erroneous, board->m, board->n)){
                printBoardIsErroneous();
                freeBoard(board);
                free(linkedList);
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
 */
struct sudokuManager* solve(struct sudokuManager *prevBoard, char *fileName){
    struct sudokuManager *board = (struct sudokuManager *) createBoardFromFile(fileName, Solve);
    if (board == NULL){ /* if board creation was unsuccessful */
        printErrorCreateBoard();
        return prevBoard;
    }
    else{
        changeMode(Solve);
        if (prevBoard != NULL) {
            freeBoard(prevBoard);
        }
    }
    return board;
}

/*
 * This function uploads a file of a game to edit.
 */
struct sudokuManager* edit(struct sudokuManager *prevBoard, char *fileName){
    struct sudokuManager *board = (struct sudokuManager *) createBoardFromFile(fileName, Edit);
    if (board == NULL){ /* board creation was unsuccessful */
        printErrorCreateBoard();
        return prevBoard; /* keep the previous board */
    }
    else{
        changeMode(Edit);
        if (prevBoard != NULL) {
            freeBoard(prevBoard); /* frees the previous board if the new boars was successful */
        }
    }
    return board;
}

/*
 * This method is used for saving your game.
 * need to validate the board!!!
 */
void saveGame(struct sudokuManager *board,char* fileName){
    FILE *file;
    int N = boardLen(board), m=board->m, n=board->n, row, col, currVal;
    if(validate(board)){
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
    }
    else{
        printBoardNotValidError(); /* MUST SAVE ONLY VALID BOARD */
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
 * this method sets Z to (X,Y),
 * is called after verifying that all values are legal.
 */
int doSet(struct sudokuManager *manager, int X, int Y, int Z){
    int prevVal = manager->board[matIndex(manager->m, manager->n, X, Y)];
    changeCellValue(manager->board, manager->m, manager->n, X, Y, Z);
    if (manager->linkedList->next != NULL){
        killNextMoves(manager);
    }
    if (createNextNode(manager, command, X, Y, Z, prevVal) == -1){
        return -1;
    }
    goToNextNode(manager);
    if (createNextNode(manager, separator, 0, 0, 0, 0) == -1){
        return -1;
    }
    goToNextNode(manager);
    return 0;
}

/*
 * Sets in board at location [Y,X] ( X column, Y row ) value Z
 */
int set(struct sudokuManager *manager, int X, int Y, int Z){
    X--, Y--;
    if(isFixedCell(manager, Y, X)){
        printErrorCellXYIsFixed(X,Y);
        return 0;
    }
    else {
        if (doSet(manager, Y, X, Z) == -1) { /* fix the order of row and column */
            return -1;
        }
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
    if(isAnyErroneousCell(board)){
        printBoardIsErroneous();
    }
    else{
        if(updateAutofillValuesBoard(board)== -1){
            return -1;
        }
        printBoard(board);
        return 0;
    }
    return 0;
}

/* GUROBI RELATED FUNCTIONS */

/*
 * This function validates the current board using ILP,
 * ensuring it's solvable.
 * returns 1 if valid and 0 otherwise.
 *  * CHECK!!!!!!!! DO WITH GUROBI
 */
int validate(struct sudokuManager *board){
    int isValid = validateBoard(board->board, board->m, board->n);
    if(!isValid){
        printBoardNotValidError();
    }
    return isValid;
}

/*
 * This function guesses a solution to the current board using LP.
 * X value is between 0 and 1.
 * validation of X and mode is done in Parser.
 */
int guess(struct sudokuManager *board, float X){
    int isValid = validate(board);
    if(!isValid){
        return -1;
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
struct sudokuManager* generate(struct sudokuManager *prevBoard, int X, int Y){
    struct sudokuManager *newBoard = (struct sudokuManager*)malloc(sizeof(struct sudokuManager));
    if(X > amountOfEmptyCells(prevBoard)){
        printGenerateInputError();
        return prevBoard;
    }
    else {
        newBoard = doGenerate(prevBoard, newBoard, X, Y);
    }
    return newBoard;
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
 * This method starts a game.
 * This method performs a whole game. It initializes the  board,
 * receives commands from the user and executes them.
 * It returns -1 when we need to terminate.
 */
int startGame(){
    char command[LENGTH];  /*Last cell is '\0'*/
    int res;
    struct sudokuManager *board;

    title();
    while (fgets(command, LENGTH, stdin) != NULL){  /* We have not reached EOF*/
        if (command[0] == '\n'){ /* Line is empty. */
            continue;
        }
        /*The line is not blank*/
        res = interpret(command, &board);
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

