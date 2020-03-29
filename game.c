#include <stdio.h>
#include <stdlib.h>
#include "main_aux.h"
#include "game.h"


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

/*
 * This function destroys all the next moves the user have done.
 * when it's done - board->linked_list->next == NULL
 * assumes the linked list is not empty
 */
void killNextMoves(struct sudokuManager *board){
    while(board->linkedList->next != NULL){ /* as long as our node does not point to NULL */
        struct movesList *tmp = board->linkedList->next; /* save a pointer to the next node */
        board->linkedList->next = board->linkedList->next->next; /* sets its' next to be our next (skip tmp) */
        free(tmp);
    }
}
/*
 * This function changes the pointer of the linked list to the first move.
 */
void pointToFirstMoveInMovesList(struct sudokuManager *board){
    while (board->linkedList->prev != NULL){
        undo(board);
    }
}

/*
 *
 */

/*
 * This method free's a new board.
 */
void freeBoard(struct sudokuManager *board){
    pointToFirstMoveInMovesList(board);
    killNextMoves(board);
    free(board->fixed);
    free(board->board);
    free(board->linkedList);
    free(board);
}

/*
 * This function loads a file and creates a sudoku board for it.
 */
struct sudokuManager* createBoardFromFile(char *fileName){
    struct sudokuManager *board = (struct sudokuManager*)malloc(sizeof(struct sudokuManager));
    board->addMarks = 1;
    struct movesList *linkedList = (struct movesList*)malloc(sizeof(struct movesList));
    linkedList->prev = NULL, linkedList->next = NULL, linkedList->prevValue = 0, linkedList->newValue = 0, linkedList->col = 0, linkedList->row = 0;
    linkedList->board = &board;
    if((mode == Edit)&&(fileName == NULL)){
        board->n = 3, board->m = 3; board->linkedList = linkedList;
        board->fixed = calloc(9 * 9, sizeof(int));
        board->board = calloc(9 * 9, sizeof(int));
    }
    else {
        FILE *ifp = NULL;
        ifp = fopen(fileName, "r");

        convertFileFormat(&board, ifp);
        /* HOW TO LOAD A FILE AND CONVERT IT ? */
        fclose(ifp);
    }
    return &board;

}

/*
 * This function updates the board to the next command
 */
void redoCommand (struct sudokuManager *board){
    while (board->linkedList->next->action != finishCommand) {
        board->linkedList = board->linkedList->next;
        board->board[matIndex(board, board->linkedList->row, board->linkedList->col)] = board->linkedList->newValue;
    }
}

/*
 * This function updates the board to the previous command
 */
void undoCommand (struct sudokuManager *board){
    while (board->linkedList->next->action != startCommand) {
        board->board[matIndex(board, board->linkedList->row, board->linkedList->col)] = board->linkedList->prevValue;
        board->linkedList = board->linkedList->prev;
    }
}

/*
 * redo a move previously undone by the user.
 */
void redo(struct sudokuManager *board){
    if (board->linkedList->next == NULL){
        printNoNextMoveError();
    }
    else {
        redoCommand(board);
    }
}

/*
 * redo a move previously undone by the user.
 */
void undo(struct sudokuManager *board){
    if (board->linkedList->prev == NULL){
        printNoPrevMoveError();
    }
    else {
        undoCommand(board);
    }
}


/*
 * This function uploads a file of a game to solve.
 */
struct sudokuManager* solve(struct sudokuManager *prevBoard, char *fileName){
    changeMode(Solve);
    struct sudokuManager *board = createBoardFromFile(fileName);
    if (board == NULL){ /* if board creation was unsuccessful */
        printErrorCreateBoard();
        free(board);
        return prevBoard;
    }
    else if (prevBoard != NULL){
        freeBoard(prevBoard);
    }
    return board;
}

/*
 * This function uploads a file of a game to edit.
 */
struct sudokuManager* edit(struct sudokuManager *prevBoard, char *fileName){
    changeMode(Edit);
    struct sudokuManager *board = createBoardFromFile(fileName);
    if (board == NULL){ /* board creation was unsuccessful */
        printErrorCreateBoard();
        free(board);
        return prevBoard; /* keep the previous board */
    }
    else if (prevBoard != NULL){
        freeBoard(prevBoard); /* frees the previous board if the new boars was successful */
    }
    return board;
}

/*
 * This function sets mark error to X
 * X and mode are being checked in Parser
 */
void markErrors(struct sudokuManager* board ,int X){
    board->addMarks = X;
}

/*
 * This function creates a new node in the linked list of moves.
 */
void createNextNode(struct sudokuManager *board, enum action action, int X, int Y, int Z, int prevVal){
    board->linkedList->next = (struct movesList*)malloc(sizeof(struct movesList)); /* allocates memory for the new node, and point our current node to the next one */
    board->linkedList->next->next = NULL;
    board->linkedList->next->prev = board->linkedList; /* points the new node's prev to the node */
    board->linkedList->next->board = board; /* sets the board pointer to our sudokuManager */
    board->linkedList->next->action = action; /* sets the given enum function action */
    if(action == command){ /* fills the values of the action changes */
        board->linkedList->col = Y, board->linkedList->row = X, board->linkedList->prevValue = prevVal, board->linkedList->newValue = Z;
    }
}

/*
 * This function goes to the next node. changes the pointer of the board to the linked list
 * Does not change the value in the board.
 */
void goToNextNode(struct sudokuManager *board){
    board->linkedList= board->linkedList->next;
}


/*
 * This function changes the value in cell <X,Y> to Z
 * assumes all input is legal
 */
void changeCellValue(struct sudokuManager *manager, int row, int col, int val){
    manager->board[matIndex(manager,row,col)] = val;
}

/*
 * this method sets Z to (X,Y),
 * is called after verifying that all values are legal.
 */
void doSet(struct sudokuManager *manager, int X, int Y, int Z){
   int prevVal =  manager->board[matIndex(manager,X,Y)];
   changeCellValue(manager, X, Y, Z);
   if (manager->linkedList->next != NULL){
        killNextMoves(manager);
   }
   createNextNode(manager, startCommand, 0, 0, 0, 0);
   goToNextNode(manager);
   createNextNode(manager, command, X, Y, Z, prevVal);
   goToNextNode(manager);
   createNextNode(manager, finishCommand, 0, 0, 0, 0);
}

/*
 * Sets in board at location [Y,X] ( X column, Y row ) value Z
 */
void set(struct sudokuManager *manager, int X, int Y, int Z){
    if(isFixedCell(manager, Y, X)){
        printErrorCellXYIsFixed(X,Y);
    } else {
        doSet(manager, Y, X, Z); /* fix the order of row and column */
    }
}


/*
 * This function validates the current board using ILP,
 * ensuring it's solvable.
 * returns 1 if valid and 0 otherwise.
 */
int validate(struct sudokuManager *board){
    int isValid = validateBoard(&(board->board), board->n, board->m);
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
void guess(struct sudokuManager *board, float X){
    int isValid = validate(board);
    if(!isValid){
        return;
    }
    /* THINK OF A WAY TO DO IT MORE EFFICIENTLY IN GUROBI SECTION! */
}

/*
 * This function generates a board with Y random cells
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
    return &newBoard;
}

/*
 * This method is used for saving your game.
 */
void saveGame(){

}

/*
 * This method is used for exiting the game.
 */
void exit(){
    printExitMessage();
    /* NOT DONE, MUST LEARN HOW TO TERMINATE */
}


void printBoard(struct sudokuManager *board){
    printSudokuGrid(board);
    /* NEED TO CHECK THE PRINTING FORMAT */
}
/*
 * This function resets the board to start position by undoing all moves.
 */
void reset(struct sudokuManager *board){
    pointToFirstMoveInMovesList(board);
}

/*
 * This method starts a game.
 */
void startGame(){
    title();
    /* NOT DONE */
}

