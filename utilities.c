#include "utilities.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>

/* GENERAL METHODS */
/*
 * This function calculates the row's lower bound of the block.
 */
int rowLowBound(struct sudokuManager *manager, int i){
    return ((i/manager->m)*manager->m);
}

/*
 * This function calculates the row's higher bound of the block.
 */
int rowHighBound(struct sudokuManager *manager, int i){
    return (((i/manager->m)+1)*manager->m);
}

/*
 * This function calculates the column's lower bound of the block.
 */
int colLowBound(struct sudokuManager *manager, int j){
    return ((j/manager->n)*manager->n);
}

/*
 * This function calculates the column's higher bound of the block.
 */
int colHighBound(struct sudokuManager *manager, int j){
    return (((j/manager->n)+1)*manager->n);
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
int matIndex(struct sudokuManager *manager, int i, int j){
    return i*boardLen(manager)+j;
}

/*
 * This method returns if a cell is a fixed cell,
 * if it is, returns 1,
 * otherwise, returns 0.
 */
int isFixedCell(struct sudokuManager *manager, int row, int col){
    return manager->fixed[matIndex(manager, row, col)];
}

/*
  * IDK what to do with this function
  */
int goOverBlock(struct sudokuManager *manager, int i, int j){
    int rowLowerBound = rowLowBound(manager, i);
    int colLowerBound = colLowBound(manager,j);
    int rowHigherBound = rowHighBound(manager, i);
    int colHigherBound = colHighBound(manager,j);
}




/*
 * Returns the amount of times that the row contains val.
 */
int rowContains(struct sudokuManager *manager, int i, int val){
    int length = boardLen(manager), col, count = 0;
    for (col = 0; col < length ; col++) {
        if(manager->board[matIndex(manager, i, col)] == val){
            count++;
        }
    }
    return count;
}

/*
 * Returns the amount of times that the column contains val
 */
int colContains(struct sudokuManager *manager, int j, int val){
    int length = boardLen(manager), row, count = 0;
    for (row = 0; row < length ; row++) {
        if(manager->board[matIndex(manager, row, j)] == val){
            count++;
        }
    }
    return count;
}

/*
 * Returns the amount of times that the block of (i,j)
 * contains val.
 */
int blockContains(struct sudokuManager *manager, int i, int j, int val){
    int blockRowLowBound = rowLowBound(manager, i);
    int blockRowHighBound = rowHighBound(manager, i);
    int blockColLowBound = colLowBound(manager, j);
    int blockColHighBound = colHighBound(manager, j);
    int row, col, count = 0;
    for(row = blockRowLowBound ; row < blockRowHighBound ; row++){
        for(col = blockColLowBound ; col < blockColHighBound ; col++) {
            if(manager->board[matIndex(manager, row, col)] == val){
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
int neighbourContains(struct sudokuManager *manager, int i, int j, int val){
    if(rowContains(manager, i, val)>1){
        return 1;
    }
    if(colContains(manager, j, val)>1){
        return 1;
    }
    if(blockContains(manager, i, j, val)>1){
        return 1;
    }
    return 0;
}

/*
 * This method returns if the value of the cell is legal,
 * if it is, returns 1,
 * otherwise, returns 0.
 */
int isLegalCell(struct sudokuManager *manager, int i, int j){
    return !neighbourContains(manager, i, j, manager->board[matIndex(manager, i, j)]);
}
