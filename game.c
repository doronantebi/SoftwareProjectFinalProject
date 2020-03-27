#include <stdio.h>
#include <stdlib.h>
#include "main_aux.h"
#include "game.h"
#include "utilities.h"

enum action{
    set = 0,
    autofillbeginning = 1,
    autofill = 2,
    autofillDone = 3
};

struct movesList{
    struct sudokuManager board;
    struct movesList *next;
    struct movesList *prev;
    int index; /* */
    int value;
    enum action action;
};



/*
 * This function loads a file and creates a sudoku board for it.
 */
struct sudokuManager* createBoardFromFile(char *fileName){
    struct sudokuManager *board = (struct sudokuManager*)malloc(sizeof(struct sudokuManager));
    /* HOW TO LOAD A FILE AND CONVERT IT ? */
    return &board;

}


void doReset(struct sudokuManager *board){
    board->linked_list = board->linked_list->next; /* going to the next element in our list */
    board->board[board->linked_list->index]=board->linked_list->value; /* setting the value in the index */
}

void reautofill (struct sudokuManager *board){
    if(board->linked_list->next != autofillbeginning){
        printErrorAutofill();
    }
    else {
        board->linked_list = board->linked_list->next;
        if(board->linked_list->next == (autofillbeginning || set)){
            printErrorAutofill();
            return;
        }
        while (board->linked_list->next->action == autofill) {
            doReset(board);
        }
        if (board->linked_list->next != autofillDone) {
            printErrorAutofill();
        } else {
            board->linked_list = board->linked_list->next;
        }
    }
}

void redo(struct sudokuManager *board){
    if (board->linked_list->next == NULL){
        printNoNextMoveError();
        return;
    }
    else {
        if(board->linked_list->next->action == set){
            doReset(board);
        }
        else {
            reautofill(board);
        }
    }
}
/*
 * This method changes the mode to the new mode
 */
void changeMode(enum Mode newMode){
    mode = newMode;
}

/*
 * This function uploads a file of a game to solve.
 */
struct sudokuManager* solve(char *fileName){
    changeMode(Solve);
    struct sudokuManager *board = createBoardFromFile(fileName);
    return board;
}

/*
 * This function uploads a file of a game to edit.
 */
struct sudokuManager* edit(char *fileName){
    changeMode(Edit);
    struct sudokuManager *board = createBoardFromFile(fileName);
    return board;
}

void markErrors(struct sudokuManager* board ,int X){
    if(mode != Solve){
        printMarkErrorsModeError();
    } else
        if ((X!=0)&&(X!=1)){
        printMarkErrorsInputError();
        }
        else {
            board->addMarks = X;
    }
}

void printBoard(struct sudokoManager *board){
    printSudokuGrid(board);
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

}

/*
 * This method is used for printing the game's title.
 */
void title(){
    printGameTitle();
}

void navigate(enum Mode mode) {

}
/*
 * This method starts a game.
 */
void startGame(){
    title();

}
