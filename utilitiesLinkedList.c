
#include <stdio.h>
#include <stdlib.h>
#include "utilitiesLinkedList.h"

/*
 * This method initializes list with default values.
 * Requires: linkedList != NULL.
 */
void initList(struct movesList *linkedList){
    linkedList->board = NULL;
    linkedList->prev = NULL;
    linkedList->next = NULL;
    linkedList->prevValue = 0;
    linkedList->newValue = 0;
    linkedList->col = 0;
    linkedList->row = 0;
    linkedList->action = separator;
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
        tmp->board = NULL;
        tmp->next = NULL;
        tmp->prev = NULL;
        free(tmp);
    }
}

/*
 * This function creates a new node in the linked list of moves.
 */
int createNextNode(struct sudokuManager *board, enum action action, int X, int Y, int Z, int prevVal){
    board->linkedList->next = (struct movesList*)malloc(sizeof(struct movesList)); /* allocates memory for the new node,
                                                                        * and point our current node to the next one */
    if (board->linkedList->next == NULL){
        return -1;
    }
    board->linkedList->next->next = NULL;
    board->linkedList->next->prev = board->linkedList; /* points the new node's prev to the node */
    board->linkedList->next->board = board; /* sets the board pointer to our sudokuManager */
    board->linkedList->next->action = action; /* sets the given enum function action */
    if(action == command){ /* fills the values of the action changes */
        board->linkedList->col = Y;
        board->linkedList->row = X;
        board->linkedList->prevValue = prevVal;
        board->linkedList->newValue = Z;
    }
    return 0;
}


/*
 * This function goes to the next node. changes the pointer of the board to the linked list
 * Does not change the value in the board.
 * assumes next != NULL
 */
void goToNextNode(struct sudokuManager *board){
    board->linkedList= board->linkedList->next;
}

/*
 * This function goes to the previous node. changes the pointer of the board to the linked list
 * Does not change the value in the board.
 * assumes prev != NULL
 */
void goToPrevNode(struct sudokuManager *board){
    board->linkedList= board->linkedList->prev;
}


/*
 * This function changes the pointer of the linked list to the first move.
 */
void pointToFirstMoveInMovesList(struct sudokuManager *board){
    if(board->linkedList == NULL){
        printf("Error: linked list is NULL(in function pointToFirstMoveInMovesList)\n");
        return;
    }
    while (board->linkedList->prev != NULL){
        undoCommand(board);
    }
}

/*
 * This function updates the board to the previous command
 */
void undoCommand (struct sudokuManager *board) {
    int m = board->m, n = board->n;
    int currVal, prevVal, row, col;
    goToPrevNode(board); /* board after every action is always at seperator, we change it to previous node so now action==command */
    while (board->linkedList->prev->action != separator) {
        row = board->linkedList->row;
        col = board->linkedList->col;
        prevVal = board->linkedList->prevValue;
        currVal = board->linkedList->newValue;
        changeCellValue(board->board, m, n, row, col, prevVal); /* sets back the previous value */
        updateEmptyCellsSingleSet(board, currVal, prevVal);
        goToPrevNode(board);
    }
}

/*
 * This function updates the board to the previous command
 */
void redoCommand (struct sudokuManager *board){
    int m = board->m, n = board->n;
    int currVal, prevVal, row, col;
    goToPrevNode(board); /* board after every action is always at finishCommand */
    while (board->linkedList->next->action != separator) {
        row = board->linkedList->row;
        col = board->linkedList->col;
        prevVal = board->linkedList->prevValue;
        currVal = board->linkedList->newValue;
        changeCellValue(board->board, m, n, row, col, currVal); /* sets back the new value*/
        updateEmptyCellsSingleSet(board, prevVal, currVal);
        goToNextNode(board);
    }
}


/*
 * This function will update the list of the board after Generate.
 */
int updateBoardLinkedList(struct sudokuManager *manager,int *retBoard){
    int row, col, length = boardLen(manager), index;
    int m = manager->m, n = manager->n;
    for(row = 0; row < length; row++){
        for(col = 0; col < length; col++){
            index = matIndex(m, n, row, col);
            if(manager->board[index] != retBoard[index]){
                if(doSet(manager, row, col, retBoard[index]) == -1){
                    return -1;
                }
            }
        }
    }
    if(createNextNode(manager, separator, 0, 0, 0, 0) == -1){
        return -1;
    }
    goToNextNode(manager);
    return 0;
}


