#ifndef SOFTWAREPROJECTFINALPROJECT_UTILITIESLINKEDLIST_H
#define SOFTWAREPROJECTFINALPROJECT_UTILITIESLINKEDLIST_H

#include "utilitiesBoardManager.h"

enum action{
    seperator = 0,
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
 * This function destroys all the next moves the user have done.
 * when it's done - board->linked_list->next == NULL
 * assumes the linked list is not empty
 */
void killNextMoves(struct sudokuManager *board);

/*
 * This function creates a new node in the linked list of moves.
 */
void createNextNode(struct sudokuManager *board, enum action action, int X, int Y, int Z, int prevVal);

/*
 * This function goes to the next node. changes the pointer of the board to the linked list
 * Does not change the value in the board.
 */
void goToNextNode(struct sudokuManager *board);


/*
 * This function changes the pointer of the linked list to the first move.
 */
void pointToFirstMoveInMovesList(struct sudokuManager *board);

/*
 * This function updates the board to the previous command
 * assumes board->linkedlist->prev != NULL
 */
void undoCommand (struct sudokuManager *board);


/*
 * This function updates the board to the next command
 * assumes board->linkedlist->next != NULL
 */
void redoCommand (struct sudokuManager *board);

#endif