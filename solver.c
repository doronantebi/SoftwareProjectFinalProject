
#include <stdio.h>
#include <stdlib.h>
#include "solver.h"
#include "utilitiesBoardManager.h"

int doNextBacktrack(struct sudokuManager *manager, int i, int j);
int recBacktracking(struct sudokuManager *manager, int *solutionBoard);

typedef struct Node {
    int value;
    int row;
    int col;
    struct Node *next;
}Node;


typedef struct Stack{
    Node *first;
}Stack;



/*
 * This method initializes the node n with the values received.
 * It sets node->next to be NULL.
 */
void initNode(Node *node, int value, int row, int col){
    node->value = value;
    node->col = col;
    node->row = row;
    node->next = NULL;
}

/*
 * This method initializes the stack s to an empty stack.
 */
void initStack(Stack *s){
    s->first = NULL;
}

/*
 * This method returns if the stack s is empty, i.e. if the first node is NULL.
 */
int isEmpty(Stack *s){
    return s->first == NULL;
}

/*
 * This method pushes a new node to the stack.
 * It returns -1 if memory allocation failed. Otherwise, it returns 0.
 */
int push(Stack *s, int value, int row, int col){
    Node *node = malloc(sizeof(Node));
     if (node == NULL){
         printAllocFailed();
         return -1;
     }
     initNode(node, value, row, col);
     if (isEmpty(s)){
        s->first = node;
    }
     else{
         node->next = s->first;
         s->first = node;
     }
     return 0;
}

/*
 * This method pops the first element out of the stack.
 * It assumes !isEmpty(s).
 */
Node *pop(Stack *s){
    Node *node = s->first;
    s->first = s->first->next;
    node->next = NULL;
    return  node;
}

/*
 * This method returns the first element of the stack.
 */
Node *top(Stack *s){
    return s->first;
}

/*
 * This method updates pRow and pCol to point to the next cell.
 * If there is no next cell, it returns 1. Otherwise, it returns 0.
 */
int getNextIndex(struct sudokuManager *manager, int *pRow, int *pCol){
    int len = boardLen(manager);

    if (isLastCellInMatrix(len, *pRow, *pCol)) { /* matrix is full and all cells are legal */
        return 1;
    }

    if (isLastInRow(len, *pCol)) { /* This is the last element in the row, now we need to begin the next row */
        *pRow = *pRow + 1;
        *pCol = 0;
        return 0;
    }
    else { /* This is not the last object in row -> we will go to the next object in row  */
        *pCol = *pCol + 1;
        return 0;
    }
}

/*
 * This method updates pRow and pCol to point to the next cell which if free.
 * If there is no free next cell, it returns 1. Otherwise, it returns 0.
 */
int findNextFreeCell(struct sudokuManager *manager, int *pRow, int *pCol){
    int m = manager->m, n = manager->n;

    while (isFixedCell(manager, *pRow, *pCol) || (manager->board[matIndex(m, n, *pRow, *pCol)] != 0)){
        if (getNextIndex(manager, pRow, pCol)){ /* There is nowhere to go ahead, all cells are full*/
            return 1;
        }
    }
    return 0;
}



/*
 * This method performs backtracking. state determines if it is deterministic or random.
 */
int recBacktracking(struct sudokuManager *manager, int *solutionBoard) {
    int nextVal, numLegalValues;
    int res, *allValues, *onlyLegalValues, *toBeFreed;
    int *usersBoard = manager->board;
    int n = manager->n;
    int m = manager->m;
    int row = 0, col = 0, numSolutions = 0, N = boardLen(manager);
    int *pRow = &row, *pCol = &col;
    Node *node;

    Stack *s = {0};

    initStack(s);

    if (findNextFreeCell(manager, pRow, pCol)){ /* There are no cells to fill */
        return numSolutions;
    }


    if (push(s, 1, row, col) == -1){ /* memory allocation failed */
        return -1;
    }

    while (!isEmpty(s) && top(s)->value < N + 1){ /* while the stack is not empty and there are more
                                                    * possible values for the current cell */
        node = top(s);

    }

    return numSolutions;



    allValues = findLegalInput(manager, row, col);  /* returns a binary array that specifies which value is
                                                            * legal to the (i, j) cell in solutionBoard */
    if (allValues == NULL){
        return 0;
    }

    numLegalValues = countZeros(allValues, boardLen(manager));     /* counts how  many legal values there
                                                                * are for the (i, j) cell*/

    if (numLegalValues == 0){
        free(allValues);
        return 0;
    }

    onlyLegalValues = malloc(numLegalValues * sizeof(int)); /* array of legal values */

    if (onlyLegalValues == NULL){
        free(allValues);
        manager->status = MALLOC_ERROR;
        return 0;
    }

    fillLegalValues(allValues, n * m, onlyLegalValues, numLegalValues);  /* fills onlyLegalValues with
                                                                        * legal values only*/

    free(allValues);

    while (numLegalValues > 1){
        nextVal = getNextVal(onlyLegalValues, numLegalValues;
        solutionBoard[matIndex(m, n, row, col)] = nextVal;
        res = doNextBacktrack(manager, row, col);
        if (res == 1){
            free(onlyLegalValues);
            return 1;
        }
        solutionBoard[row * (m * n) + col] = 0;
        toBeFreed = removeFromArray(&onlyLegalValues, numLegalValues, nextVal);
        if (toBeFreed == NULL){ /* realloc failed */
            free(onlyLegalValues);
            manager->status = REALLOC_ERROR;
            return 0;
        }
        onlyLegalValues = toBeFreed;
        numLegalValues--;
    }

    if (numLegalValues == 1){
        nextVal = onlyLegalValues[0];
        solutionBoard[row * (m * n) + col] = nextVal;
        free(onlyLegalValues);
        res = doNextBacktrack(manager, row, col);
        if (res == 1){
            return 1;
        }
        solutionBoard[matIndex(m, n, row, col)] = 0;
    }

    return 0;

}

/*
 *
 */
int backtracking(struct sudokuManager *manager){
    int res;
    manager->status = CAN_BE_SOLVED;
    res = recBacktracking(manager, 0, 0);
    if (manager->status == CAN_BE_SOLVED){
        manager->status = CANNOT_BE_SOLVED;
    }
    return res;
}


