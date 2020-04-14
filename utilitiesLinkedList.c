
#include <stdio.h>
#include <stdlib.h>
#include "utilitiesLinkedList.h"
#include "main_aux.h"


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
        board->linkedList->next->col = Y;
        board->linkedList->next->row = X;
        board->linkedList->next->prevValue = prevVal;
        board->linkedList->next->newValue = Z;
    }
    return 0;
}


/*
 * This function goes to the next node. changes the pointer of the board to the linked list
 * Does not change the value in the board.
 * assumes next != NULL
 */
void goToNextNode(struct sudokuManager *board){
    board->linkedList = board->linkedList->next;
}

/*
 * This function goes to the previous node. changes the pointer of the board to the linked list
 * Does not change the value in the board.
 * assumes prev != NULL
 */
void goToPrevNode(struct sudokuManager *board){
    board->linkedList = board->linkedList->prev;
}


/*
 * This function changes the pointer of the linked list to the first move.
 */
void pointToFirstMoveInMovesList(struct sudokuManager *board, int isToPrint){
    if(board->linkedList == NULL){
        printf("Error: linked list is NULL(in function pointToFirstMoveInMovesList)\n");
        return;
    }
    while (board->linkedList->prev != NULL){
        undoCommand(board, isToPrint);
    }
}

/*
 * This function updates the board to the previous command
 * assumes board->linkedlist->prev != NULL
 * It returns the number of the cells we changed.
 */
int undoCommand (struct sudokuManager *board, int isToPrint) {
    int m = board->m, n = board->n, count = 0;
    int currVal, prevVal, row, col;
    goToPrevNode(board); /* board after every action is always at seperator,
                        * we change it to previous node so now action==command */
    while (board->linkedList->action != separator) {
        count++;
        row = board->linkedList->row;
        col = board->linkedList->col;
        prevVal = board->linkedList->prevValue;
        currVal = board->linkedList->newValue;
        changeCellValue(board->board, m, n, row, col, prevVal); /* sets back the previous value */
        updateEmptyCellsSingleSet(board, currVal, prevVal);
        if(isToPrint){
            printActionWasMade(row, col, currVal, prevVal);
        }
        goToPrevNode(board);
    }
    return count;
}

/*
 * This function updates the board to the previous command
 * assumes board->linkedlist->next != NULL
 * It returns the number of the cells we changed.
 */
int redoCommand (struct sudokuManager *board, int isToPrint){
    int m = board->m, n = board->n, count = 0;
    int currVal, prevVal, row, col;
    goToNextNode(board); /* board after every action is always at finishCommand */
    while (board->linkedList->action != separator) {
        count++;
        row = board->linkedList->row;
        col = board->linkedList->col;
        prevVal = board->linkedList->prevValue;
        currVal = board->linkedList->newValue;
        changeCellValue(board->board, m, n, row, col, currVal); /* sets back the new value*/
        updateEmptyCellsSingleSet(board, prevVal, currVal);
        if(isToPrint){
            printActionWasMade(row, col, prevVal, currVal);
        }
        goToNextNode(board);
    }
    return count;
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


