
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
 */
void push(Stack *s, Node *node){
     if (isEmpty(s)){
        s->first = node;
    }
     else{
         node->next = s->first;
         s->first = node;
     }
}

/*
 * This method pops the first element out of the stack.
 * It assumes !isEmpty(s).
 */
void pop(Stack *s){
    Node *node = s->first;
    s->first = s->first->next;
    node->next = NULL;
    free(node);
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
 * Namely, it returns 1 iff the board is completely full.
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
 * This method finds the next legal value in solutionBoard for cell (row, col)
 */
int findNextLegalValue(int m, int n, int row, int col, int *solutionBoard){
    int i;
    int currValue = solutionBoard[matIndex(m, n, row, col)];
    for (i = currValue + 1; i <= n*m; i++){
        if (!neighbourContains(solutionBoard, m, n, row, col, i)){
            return i;
        }
    }
    return i;
}


/*
 * This method performs backtracking. state determines if it is deterministic or random.
 */
int recBacktracking(struct sudokuManager *manager, int *solutionBoard) {
    int n = manager->n;
    int m = manager->m;
    int row = 0, col = 0, numSolutions = 0, N = boardLen(manager), value;
    int *pRow = &row, *pCol = &col;
    Node *node;

    Stack *s = (Stack *) malloc(sizeof(Stack));
    if (s == NULL) {
        printAllocFailed();
        return -1;
    }

    initStack(s);

    if (findNextFreeCell(manager, pRow, pCol)) { /* There are no cells to fill */
        return 0;
    }

    value = findNextLegalValue(m, n, row, col, solutionBoard);

    if (value == N + 1) { /* the board is invalid */
        return 0;
    }

    node = (Node *) malloc(sizeof(Node));
    if (node == NULL) { /* memory allocation failed */
        printAllocFailed();
        return -1;
    }

    initNode(node, value, *pRow, *pCol);
    push(s, node);

    changeCellValue(solutionBoard, m, n, row, col, value);

    while (!isEmpty(s)) { /* while the stack is not empty and there are more
                                                    * possible values for the current cell */
        node = top(s);
        *pRow = node->row;
        *pCol = node->col;

        /*if (isLastCellInMatrix(N, node->row, node->col)) {
            numSolutions++;
            changeCellValue(solutionBoard, m, n, node->row, node->col, 0); if we filled the last cell,
                                                                                * it has only one legal value
            pop(s);

        } else {*/
            getNextIndex(manager, pRow, pCol); /* we can't be in the last cell of the matrix */
            if (findNextFreeCell(manager, pRow, pCol)) { /* */
                numSolutions++;
                value = findNextLegalValue(m, n, node->row, node->col, solutionBoard);
                if (value == N + 1) {
                    changeCellValue(solutionBoard, m, n, node->row, node->col, 0);
                    pop(s);
                } else {
                    changeCellValue(solutionBoard, m, n, node->row, node->col, value);
                }
            } else {
                value = findNextLegalValue(m, n, *pRow, *pCol, solutionBoard);
                if (value == N + 1) {
                    changeCellValue(solutionBoard, m, n, node->row, node->col, 0);
                    pop(s);
                } else { /* value < N + 1*/
                    node = (Node *) malloc(sizeof(Node));
                    if (node == NULL) {
                        printAllocFailed();
                        return -1;
                    }
                    initNode(node, value, *pRow, *pCol);
                    changeCellValue(solutionBoard, m, n, node->row, node->col, value);
                    push(s, node);
                }
            }
    }

    free(s);
    return numSolutions;
}



/*
 * This method returns the number of possible solutions of the current board using the backtracking algorithm.
 * If there is memory allocation error, it returns -1.
 */
int backtracking(struct sudokuManager *manager){
    int res, len = boardLen(manager), *solutionBoard;
    if (isAnyErroneousCell(manager)){
        return 0;
    }

    solutionBoard = (int *)calloc(boardArea(manager), sizeof(int));
    if (solutionBoard == NULL){
        printAllocFailed();
        return -1;
    }

    copyBoard(solutionBoard, manager->board, len);
    res = recBacktracking(manager, solutionBoard);

    return res;
}


