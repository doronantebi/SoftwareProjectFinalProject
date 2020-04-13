
#include <stdio.h>
#include <stdlib.h>
#include "solver.h"
#include "utilitiesBoardManager.h"
#include "main_aux.h"
#include "gurobi.h"

#define NUM_ITERATIONS 1000


double randRangeDouble(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}


int randRangeInt(int min, int max)
{
    int range = (max - min);
    return min + (rand() % range);
}

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
        if (!neighbourContainsOnce(solutionBoard, m, n, row, col, i)){
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
    int res, *solutionBoard;
    if (isAnyErroneousCell(manager)){
        return 0;
    }

    solutionBoard = (int *)calloc(boardArea(manager), sizeof(int));
    if (solutionBoard == NULL){
        printAllocFailed();
        return -1;
    }

    duplicateBoard(solutionBoard, manager->board, manager->m, manager->n);
    res = recBacktracking(manager, solutionBoard);

    return res;
}



/*
 * This function validates a board using ILP
 * returns 1 if valid and 0 if it isn't .
 * returns -1 if allocation failed
 * returns -2 if gurobi error occurred.
 */
int validateBoard(struct sudokuManager *manager){
    int res;
    int *retBoard = calloc(boardArea(manager),sizeof(int));
    if(retBoard == NULL){
        return -1;
    }
    res = solveBoard(manager, &retBoard);
    if(res == -1){ /* gurobi error */
        return -2;
    }
    else {
        if (res == -2) { /* memory allocation error */
            return -1;
        } else {
            if (retBoard == NULL) {
                /* the board is not valid */
                return 0;
            } else {
                return 1;
            }
        }
    }
}

/*
 * This method guesses a hint for cell (row, col) using LP.
 *  The method returns -1 there was a nonfatal error because of which we can't execute the command and need to continue,
 *  -2 if memory allocation failed, and 0 otherwise.
 *  User needs to free *pCellValues iff return value == 0.
 */
int doGuessHint(struct sudokuManager *manager, int row, int col, int **pCellValues, int *pLength) {
    /* initializing *pCellValues and *pLength */
    *pCellValues = NULL;
    *pLength = 0;
    return guessCellValues(manager, row, col, pCellValues, pLength);
}

/*
 * This function fills X random cells with legal values
 */
void doGenerateFillNumRandomCells(struct sudokuManager *board, int *newBoard, int cellsToFill){
    int row, col, val, m = board->m, n = board->n;
    while (cellsToFill > 0) {
        row = randRangeInt(0, boardLen(board)); /* MAKE SURE!!!!!! IT DOESNT INCLUDE THE UPPER BOUND */
        col = randRangeInt(0, boardLen(board));
        if (newBoard[matIndex(m, n, row, col)] == 0) { /* if cell is empty */
            val = randRangeInt(0, boardLen(board)) + 1;
            while (neighbourContainsOnce(newBoard, m, n, row, col, val)) { /* as long as val is
                                                                                * illegal for our curr cell */
                val = randRangeInt(0, boardLen(board)) + 1; /* randomly choose different value  */
            }
            changeCellValue(newBoard, m, n, row, col, val); /* set the new value */
            cellsToFill--; /* reduce X by one */
        }
    }
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
 * This function generates a new board with Y cells.
 * if has been successful, returns a pointer to a newBoard that will have Y values.
 * if fails, returns board. If we need to terminate, it returns NULL.
 */
int* doGenerate(struct sudokuManager *board, int *newBoard, int X, int Y){
    /* ALL ALLOCATIONS */
    struct sudokuManager *newManager = (struct sudokuManager*)malloc(sizeof(struct sudokuManager));
    int* retBoard = (int*)calloc(boardArea(board), sizeof(int)); /* THIS WILL CONTAIN THE SOLUTION */
    int* erroneous = (int*)calloc(boardArea(board), sizeof(int));
    int* fixed = (int*)calloc(boardArea(board), sizeof(int));
    struct movesList *list = (struct movesList*) malloc(sizeof(struct movesList));
    int retGurobi;
    int m = board->m, n = board->n, iter;

    initNullBoard(newManager);

    if((newManager == NULL)||(retBoard == NULL) || (erroneous == NULL) || (fixed == NULL) || (list == NULL)){
        free(newManager);
        free(retBoard);
        free(erroneous);
        free(list);
        free(fixed);
        return NULL;
    }

    srand(time(NULL));

    /* INITIALIZES NEW SUDOKU MANAGER */
    initBoardValues(newManager, m, n, board->board,erroneous, fixed, board->addMarks, board->emptyCells, list);

    /* STARTING 1000 ITERETIONS */
    for(iter = 0; iter < NUM_ITERATIONS; iter ++) {

        duplicateBoard(board->board, newBoard, board->m, board->n); /* copy content of prevBoard to newBoard */

        /* RANDOMLY FILLS X CELLS */
        doGenerateFillNumRandomCells(board, newBoard, X); /* this function fills random legal cells */

        newManager->board = newBoard; /* solve the board with the new X filled cells */
        /* previous board is saved in prevBoard */
        retGurobi = solveBoard(newManager, &retBoard);
        if(retGurobi == -2){ /* allocation failed... */
            return NULL;
        }
        else {
            if(retGurobi == 0){ /* Gurobi error did not occur */
                if(retBoard != NULL){ /* solution has been found!!! Hurray!!! */
                    doGenerateRemoveNumRandomCells(board, retBoard, boardArea(board) - Y);
                    /* This function removes cells */

                    freeBoard(newManager);
                    return retBoard;
                }
            }
        }
    }
    freeBoard(newManager);
    free(retBoard);
    return board->board; /* after 1000 trys we will returns the previous board */
}

/*
 * This method guesses a solution for the entire board and fills its cells with this solution if legal.
 * The method returns -2 there was a nonfatal error because of which we can't execute the command (gurobi failed)
 * and need to continue, -1 if memory allocation failed, and 0 otherwise.
 */
int doGuess(struct sudokuManager *manager, float threshold){
    int res;
    res = guessSolution(manager, threshold);
    if (res == -2){
        return -1;
    }
    if (res == -1){
        return -2;
    }
    return 0;
}


/*
 * This function fills in *hint a hint for cell <row,col>
 * returns 1 if succeed solving the board.
 * returns -1 if allocation failed
 * returns -2 if gurobi failed.
 * returns 0 if board could not be solved or gurobi failed
 */
int getHint(struct sudokuManager *manager, int row, int col, int* hint){
    int res;
    int *retBoard = calloc(boardArea(manager),sizeof(int));
    if(retBoard == NULL){
        printAllocFailed();
        return -2;
    }
    res = solveBoard(manager, &retBoard);
    if(res == -1){
        free(retBoard);
        return -2;
    }
    else{
        if(res == -2){ /* alloc failed in gurobi  */
            return -1;
        }
        else{
            if(retBoard == NULL){/* the board is not valid */
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
