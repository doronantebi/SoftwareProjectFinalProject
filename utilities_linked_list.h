/*
 * This module deals with everything that has to do with the movesList struct.
 * It have methods regarding this struct which can be used by any module which includes utilitiesLinkedList.c
 * and uses movesList struct.
 */

#ifndef SOFTWAREPROJECTFINALPROJECT_LINKED_LIST_H
#define SOFTWAREPROJECTFINALPROJECT_LINKED_LIST_H

#include "utilities_board_manager.h"

enum action{
    separator = 0,
    command = 1
};

struct movesList{
    struct sudokuManager *board;
    struct movesList *next;
    struct movesList *prev;
    int row;
    int col;/* */
    int prevValue;
    int newValue;
    enum action action;
};

/*
 * This method initializes list with default values.
 * Requires: linkedList != NULL.
 */
void initList(struct movesList *linkedList);


/*
 * This function destroys all the next moves the user have done.
 * when it's done - board->linked_list->next == NULL
 * assumes the linked list is not empty.
 */
void killNextMoves(struct sudokuManager *board);

/*
 * This function creates a new node in the linked list of moves.
 * Return values:
 * -1: memory allocation failed.
 *  0: node has been created successfully.
 */
int createNextNode(struct sudokuManager *board, enum action action, int X, int Y, int Z, int prevVal);

/*
 * This function goes to the next node.
 * changes the pointer of the board to the linked list,
 * Does not change the value in the board.
 */
void goToNextNode(struct sudokuManager *board);

/*
 * This function changes the pointer of the linked list to the first move.
 */
void pointToFirstMoveInMovesList(struct sudokuManager *board, int isToPrint);

/*
 * This function updates the board to the previous command,
 * assumes board->linkedlist->prev != NULL
 * Return values: returns the number of cells we changed.
 */
int undoCommand (struct sudokuManager *board, int isToPrint);

/*
 * This function updates the board to the next command,
 * assumes board->linkedlist->next != NULL
 * Return values: returns the number of cells we changed.
 */
int redoCommand (struct sudokuManager *board, int isToPrint);

/*
 * This function will update the list of the board after "generate" and "guess".
 * Return values:
 * -1: memory allocation failed.
 *  0: update succeeded.
 */
int updateBoardLinkedList(struct sudokuManager *manager,int *retBoard);

#endif
