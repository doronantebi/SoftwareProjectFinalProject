
#include <stdio.h>
#include <stdlib.h>
#include "solver.h"
#include "utilitiesBoardManager.h"
#include "main_aux.h"
#include "gurobi.h"

#define NUM_ITERATIONS 1000

/* VALIDATE */

/*
 * This function validates a board using ILP.
 * Return values:
 * -2: Gurobi failure.
 * -1: memory allocation failed.
 *  0: the board is invalid.
 *  1: the board is valid.
 */
int validateBoard(struct sudokuManager *manager){
    int res;
    int *retBoard = calloc(boardArea(manager),sizeof(int));
    if(retBoard == NULL){
        return -1;
    }
    res = solveBoard(manager, &retBoard);
    free(retBoard);
    if(res == -1){ /* gurobi error */
        return -2;
    }
    if (res == -2) { /* memory allocation error */
        return -1;
    }
    return res;
}

/* GUESS */

/*
 * This function guesses a solution for the entire board and
 * fills its cells with values with higher probability than [threshold]
 * to appear in a solution to the board.
 * Return values:
 * -2: Gurobi failure.
 * -1: memory allocation failed.
 *  0: The board is invalid.
 *  1: The board is valid and the guessed solution is filled in retBoard.
 */
int doGuess(struct sudokuManager *manager, float threshold, int *retBoard){
    int res;
    res = guessSolution(manager, threshold, retBoard);
    if (res == -2){
        return -1;
    }
    if (res == -1){
        return -2;
    }
    return res;
}

/* GENERATE */

/*
 * This function raffles an integer between min an max.
 */
int randRangeInt(int min, int max)
{
    int range = (max - min);
    return min + (rand() % range);
}

/*
 * This function fills X random cells with legal values.
 * It returns 0 if there is no legal value for some index raffled.
 * If all raffled indices had legal values, it returns 1.
 */
int doGenerateFillNumRandomCells(struct sudokuManager *board, int *newBoard, int cellsToFill){
    int row, col, val, m = board->m, n = board->n;
    while (cellsToFill > 0) {
        row = randRangeInt(0, boardLen(board)); /* MAKE SURE!!!!!! IT DOESNT INCLUDE THE UPPER BOUND */
        col = randRangeInt(0, boardLen(board));
        if (newBoard[matIndex(m, n, row, col)] == 0) { /* if cell is empty */
            if (returnLegalValue(board->board, m, n, row, col) == -1){
                return 0;
            }
            val = randRangeInt(0, boardLen(board)) + 1;
            while (neighbourContainsOnce(newBoard, m, n, row, col, val)) { /* as long as val is
                                                                                * illegal for our curr cell */
                val = randRangeInt(0, boardLen(board)) + 1; /* randomly choose different value  */
            }
            changeCellValue(newBoard, m, n, row, col, val); /* set the new value */
            cellsToFill--; /* reduce X by one */
        }
    }
    return 1;
}

/*
 * This function removes from retBoard cellsToRemove cells
 */
void doGenerateRemoveNumRandomCells(struct sudokuManager *board, int *retBoard, int cellsToRemove){
    int row, col, m = board->m, n = board->n;
    while (cellsToRemove > 0){
        row = randRangeInt(0, boardLen(board));
        col = randRangeInt(0, boardLen(board));
        if(retBoard[matIndex(m, n, row, col)] != 0){
            changeCellValue(retBoard, m, n, row, col, 0);
            cellsToRemove --;
        }
    }
}

/*
 * This function updates a retBoard to contain only Y cells.
 *  This method returns -1 if memory allocation failed,
 *  1 if board is solvable,
 *  and 0 if we didn't succeed in generating Y cells.
 *  if return value == 1, retBoard will have Y values.
 */
int doGenerate(struct sudokuManager *board, int X, int Y, int *retBoard){
    /* ALL ALLOCATIONS */
    struct sudokuManager *newManager = (struct sudokuManager*)malloc(sizeof(struct sudokuManager));
    int* erroneous = (int*)calloc(boardArea(board), sizeof(int));
    int* fixed = (int*)calloc(boardArea(board), sizeof(int));
    struct movesList *list = (struct movesList*) malloc(sizeof(struct movesList));
    int *newBoard = (int*)calloc(boardArea(board), sizeof(int));
    int retGurobi;
    int m = board->m, n = board->n, iter;

    initNullBoard(newManager);

    if((newManager == NULL) || (retBoard == NULL) || (erroneous == NULL)
       || (fixed == NULL) || (list == NULL) || (newBoard == NULL)){
        free(newManager);
        free(retBoard);
        free(erroneous);
        free(list);
        free(fixed);
        free(newBoard);
        return -1;
    }

    srand(time(NULL));

    /* INITIALIZES NEW SUDOKU MANAGER */
    initBoardValues(newManager, m, n, newBoard, erroneous, fixed, board->emptyCells, list);

    /* STARTING 1000 ITERETIONS */
    for(iter = 0; iter < NUM_ITERATIONS; iter ++) {

        duplicateBoard(board->board, newBoard, board->m, board->n); /* copy content of board->board to newBoard */

        /* RANDOMLY FILLS X CELLS */
        if (doGenerateFillNumRandomCells(board, newBoard, X) == 0) { /* this function fills random legal cells */
            continue; /* if a raffled index had no illegal values, we need to try again */
        }

        retGurobi = solveBoard(newManager, &retBoard); /* solve the board with the new X filled cells */
        if(retGurobi == -2){ /* allocation failed... */
            return -1;
        }
        else {
            if(retGurobi == 1){ /* solution has been found!!! Hurray!!! */
                doGenerateRemoveNumRandomCells(board, retBoard, boardArea(board) - Y);
                /* This function removes cells */
                freeBoard(newManager);
                return 1;
            }
        }
    }
    freeBoard(newManager);
    return 0; /* after 1000 trys we return we didn't succeed*/
}

/* HINT */

/*
 * This function fills in *hint a hint for cell <row,col>
 * returns 1 if succeeded solving the board.
 * returns -1 if allocation failed
 * returns -2 if gurobi failed.
 * returns 0 if board could not be solved
 */
int getHint(struct sudokuManager *manager, int row, int col, int* hint){
    int res;
    int *retBoard = calloc(boardArea(manager),sizeof(int));
    if(retBoard == NULL){
        return -1;
    }
    res = solveBoard(manager, &retBoard);
    if(res == -1){
        free(retBoard);
        return -2;
    }
    else{
        if(res == -2){ /* alloc failed in gurobi  */
            free(retBoard);
            return -1;
        }
        else{
            if(res == 0){/* the board is not valid */
                free(retBoard);
                return 0;
            }
            else {
                *hint = retBoard[matIndex(manager->m, manager->n, row, col)];
                free(retBoard);
                return 1;
            }
        }
    }
}

/* GUESS_HINT */

/*
 * This function guesses a hint for cell (row, col) using LP.
 *  Return values:
 *  -2: there was a nonfatal error because of which we can't execute
 *      the command and need to continue.
 *  -1: memory allocation failed.
 *   1: the board is solvable.
 *   0: the board is unsolvable.
 *  User needs to free *pCellValues iff return value == 1.
 */
int doGuessHint(struct sudokuManager *manager, int row, int col, int **pCellValues, double **pScores, int *pLength) {
    int res;
    /* initializing *pCellValues and *pLength */
    *pCellValues = NULL;
    *pLength = 0;
    res = guessCellValues(manager, row, col, pCellValues, pScores, pLength);
    if (res == -1){ /* Gurobi error */
        return -2;
    }

    if (res == -2){ /* memory allocation error */
        return -1;
    }

    return res;
}

/* NUM_SOLUTIONS */

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
 * This method initializes the node with the values received.
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
 * This method returns if the stack s is empty,
 * i.e. if the first node is NULL.
 */
int isEmpty(Stack *s){
    return (s->first == NULL);
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
        if (!neighbourContainsOnce(solutionBoard, m, n, row, col, i)){
            return i;
        }
    }
    return i;
}

/*
 * This method performs backtracking.
 */
int recBacktracking(struct sudokuManager *manager, int *solutionBoard) {
    int n = manager->n;
    int m = manager->m;
    int row = 0, col = 0, numSolutions = 0, N = boardLen(manager), value;
    int *pRow = &row, *pCol = &col;
    Node *node;

    Stack *s = (Stack *) malloc(sizeof(Stack));
    if (s == NULL) {
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
        return -1;
    }

    initNode(node, value, *pRow, *pCol);
    push(s, node);

    changeCellValue(solutionBoard, m, n, row, col, value);

    while (!isEmpty(s)) { /* while the stack is not empty and there are more
                        * possible values for the current cell */

        node = top(s);

        if (node->value == N + 1){ /* checking if there are more possible values for the current cell.
                                    *node->value == N + 1 iff there are no more legal values to check for this cell*/
            changeCellValue(solutionBoard, m, n, node->row, node->col, 0); /*deleting the value in this cell*/
            pop(s); /*deleting the node from the stack*/
            if (!isEmpty(s)){ /*if the stack is not empty, we need to advance the value of the next node*/
                node = top(s);
                value = findNextLegalValue(m, n, node->row, node->col, solutionBoard);
                changeCellValue(solutionBoard, m, n, node->row, node->col, value);
                node->value = value;
            }
            continue; /*continuing to the next iteration*/
        }

        *pRow = node->row;
        *pCol = node->col;

        printf("cell is (%d, %d)\n", *pRow, *pCol);

        if (isLastCellInMatrix(N, node->row, node->col)) {
            numSolutions++;
            printf("num_solutions is %d\n", numSolutions);
            value = findNextLegalValue(m, n, node->row, node->col, solutionBoard);
            changeCellValue(solutionBoard, m, n, node->row, node->col, value);
            node->value = value;
        }
        else{
            getNextIndex(manager, pRow, pCol); /* going to the next cell. If we are the last cell
                                            * in the matrix, we stay put */
            if (findNextFreeCell(manager, pRow, pCol)) { /* There are no more cells to fill */
                numSolutions++;
                printf("num_solutions is %d\n", numSolutions);
                value = findNextLegalValue(m, n, node->row, node->col, solutionBoard);
                changeCellValue(solutionBoard, m, n, node->row, node->col, value);
                node->value = value;
            } else { /*cell (*pRow, *pCol) needs to be filled*/
                value = findNextLegalValue(m, n, *pRow, *pCol, solutionBoard); /*finding value for (*pRow, *pCol)*/
                node = (Node *) malloc(sizeof(Node));
                if (node == NULL) {
                    printAllocFailed();
                    return -1;
                }
                initNode(node, value, *pRow, *pCol);
                changeCellValue(solutionBoard, m, n, node->row, node->col, value);
                push(s, node); /*pushing this cell to the stack*/
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
    int res, *solutionBoard;
    if (isAnyErroneousCell(manager)){
        return 0;
    }

    solutionBoard = (int *)calloc(boardArea(manager), sizeof(int));
    if (solutionBoard == NULL){
        return -1;
    }

    duplicateBoard(manager->board, solutionBoard, manager->m, manager->n);

    res = recBacktracking(manager, solutionBoard);

    free(solutionBoard);

    return res;
}