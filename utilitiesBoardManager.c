#include "utilitiesBoardManager.h"
#include <stdlib.h>
#include <stdio.h>

/* GENERAL METHODS */


/*
 * This function calculates the row's lower bound of the block.
 * CHECK!!!!!!!!
 */
int rowLowBound(int m, int i){
    return ((i/m)*m);
}

/*
 * This function calculates the row's higher bound of the block.
 * CHECK!!!!!!!!
 */
int rowHighBound(int m, int i){
    return (((i/m)+1)*m);
}

/*
 * This function calculates the column's lower bound of the block.
 * CHECK!!!!!!!!
 */
int colLowBound(int n, int j){
    return ((j/n)*n);
}

/*
 * This function calculates the column's higher bound of the block.
 * CHECK!!!!!!!!
 */
int colHighBound(int n, int j){
    return (((j/n)+1)*n);
}


/*
 * This method returns the length of the sudoku board
 */
int boardLen(struct sudokuManager *manager){
    return (manager->m)*(manager->n);
}

/*
 * returns the amount of cells in the board
 */
int boardArea(struct sudokuManager *manager){
    return (boardLen(manager)*boardLen(manager));
}

/*
 * This method returns the matrix index in the array of the board
 */
int matIndex(int m, int n, int i, int j){
    return i*(n*m)+j;
}

/*
 * This method returns if a cell is a fixed cell,
 * if it is, returns 1,
 * otherwise, returns 0.
 */
int isFixedCell(struct sudokuManager *manager, int row, int col){
    return manager->fixed[matIndex(manager->m, manager->n, row, col)];
}


/*
 * Returns the amount of times that the row contains val.
 * CHECK!!!!!!!!
 */
int rowContains(int *board, int m, int n, int i, int val){
    int length = n*m, col, count = 0;
    for (col = 0; col < length ; col++) {
        if(board[matIndex(m, n, i, col)] == val){
            count++;
        }
    }
    return count;
}

/*
 * Returns the amount of times that the column contains val
 */
int colContains(int *board, int m, int n, int j, int val){
    int length = n*m, row, count = 0;
    for (row = 0; row < length ; row++) {
        if(board[matIndex(m, n, row, j)] == val){
            count++;
        }
    }
    return count;
}

/*
 * Returns the amount of times that the block of (i,j)
 * contains val.
 */
int blockContains(int* board, int m, int n, int i, int j, int val){
    int blockRowLowBound = rowLowBound(m, i);
    int blockRowHighBound = rowHighBound(m, i);
    int blockColLowBound = colLowBound(n, j);
    int blockColHighBound = colHighBound(n, j);
    int row, col, count = 0;
    for(row = blockRowLowBound ; row < blockRowHighBound ; row++){
        for(col = blockColLowBound ; col < blockColHighBound ; col++) {
            if(board[matIndex(m, n, row, col)] == val){
                count++;
            }
        }
    }
    return count;
}

/*
 * This method returns 1 if a row/col/block contains
 * val more than once.
 */
int neighbourContains(int* board, int m, int n, int i, int j, int val){
    if(rowContains(board, m, n, i, val)>1){
        return 1;
    }
    if(colContains(board, m, n, j, val)>1){
        return 1;
    }
    if(blockContains(board, m, n, i, j, val)>1){
        return 1;
    }
    return 0;
}

int updateErroneousBlock(int* board, int* erroneous, int m, int n, int i, int j){
    int blockRowLowBound = rowLowBound(m, i);
    int blockRowHighBound = rowHighBound(m, i);
    int blockColLowBound = colLowBound(n, j);
    int blockColHighBound = colHighBound(n, j);
    int row, col;
    for(row = blockRowLowBound; row < blockRowHighBound; row++){
        for(col = blockColLowBound ; col < blockColHighBound ; col++) {
            if(neighbourContains(board, m, n, row, col, board[matIndex(m, n, row, col)])){
                erroneous[matIndex(m, n, row, col)] = 1;
            }
            else {
                erroneous[matIndex(m, n, row, col)] = 0;
            }
        }
    }
    return 1;
}

int updateErroneousRow(int* board, int* erroneous, int m, int n, int j){
    int row, length = n*m;
    for (row = 0; row < length; row++) {
        if(neighbourContains(board, m, n, row, j, board[matIndex(m, n, row, j)])){
            erroneous[matIndex(m, n, row, j)] = 1;
        }
        else {
            erroneous[matIndex(m, n, row, j)] = 0;
        }
    }
    return 1;
}
int updateErroneousCol(int* board, int* erroneous, int m, int n, int i){
    int col, length = n*m;
    for (col = 0; col < length; col++) {
        if(neighbourContains(board, m, n, i, col, board[matIndex(m, n, i, col)])){
            erroneous[matIndex(m, n, i, col)] = 1;
        }
        else {
            erroneous[matIndex(m, n, i, col)] = 0;
        }
    }
    return 1;
}

/*
 * This updates the erroneous board for a sudokuBoard
 * input is a pointer to the board typed *int.
 * Will be used when Loading a file in Edit mode
 * or when loading a file in Solve mode when setting addMarks to be 1
 * UPDATES THE WHOLE BOARD
 */
int updateErroneousBoard(int* board, int* erroneous, int m, int n){
    int row, col, length = n*m;
    for(row = 0; row < length; row++){
        for (col = 0; col < length; col++) {
            if(neighbourContains(board, m, n, row, col, board[matIndex(m, n, row, col)])){
                erroneous[matIndex(m, n, row, col)] = 1;
            }
            else {
                erroneous[matIndex(m, n, row, col)] = 0;
            }
        }
    }
    return 1;
}

/*
 * This function updates the erronous board of a sudokuManager.
 * it will update all row i, column j, and the block that (i,j) is in.
 */
int updateErroneousBoardCell(int* board, int* erroneous, int m, int n, int i, int j){
    updateErroneousBlock(board, erroneous, m, n, i,j);
    updateErroneousCol(board, erroneous, m, n, i);
    updateErroneousRow(board, erroneous, m, n, j);
    return 1;
}

/*
 * This method returns if the value of the cell is legal,
 * if it is, returns 1,
 * otherwise, returns 0.
 */
int isErroneous(struct sudokuManager *manager, int i, int j){
    return manager->erroneous[matIndex(manager->m, manager->n, i, j)];
}

/*
 * Checks that the val
 */
int isLegalCellValue(struct sudokuManager *manager, int x){
    return ((x >= 0) && (x <= boardLen(manager)));
}

/*
 * counts the empty cells in the board
 */
int amountOfEmptyCells(struct sudokuManager *manager){
    int i, count = 0;
    for(i=0; i<boardArea(manager); i++){
        if(manager->board[i] == 0)
            count ++;
    }
    return count;
}


/*
 * This method frees a given board.
 */
void freeBoard(struct sudokuManager *board){
    pointToFirstMoveInMovesList(board); /* points the move's list to first move */
    killNextMoves(board); /* frees all next moves */
    free(board->fixed);
    free(board->erroneous);
    free(board->board);
    free(board->linkedList);
    free(board);
}


/*
 * This function changes the value in cell <X,Y> to Z
 * assumes all input is legal
 */
void changeCellValue(int *board, int m, int n, int row, int col, int val){
    board[matIndex(m, n,row,col)] = val;
}

/*
 * This method creates a new matrix of integers containing only the fixed values in board.
 * If there is an error allocating memory, it returns NULL and prints a message to the user.
 */
int *copyFixedOnly(struct sudokuManager *board, int *onlyFixed){
    int i, j, index;

    for (i = 0; i < boardLen(board); i++) { /*Row*/
        for (j = 0; j < boardLen(board); j++){ /*Column*/
            if (isFixedCell(board, i, j)){
                index = matIndex(board->m, board->n, i, j);
                changeCellValue(onlyFixed, board->m, board->n, i, j, board->board[index]);
            }
        }
    }

    return onlyFixed;
}

/*
 * This method checks if the current cell in last is row, assuming length of row is size.
 */
int isLastInRow(int size, int j){
    return j == size - 1;
}

/*
 * This method checks if the current cell is last in column, assuming length of column is size.
 */
int isLastInCol(int size, int i){
    return i == size - 1;
}


/*
* This method checks if the current cell is last in the matrix, assuming the matrix is size X size.
*/
int isLastCellInMatrix(int size, int i, int j){
    return isLastInCol(size, i) && isLastInRow(size, j);
}

