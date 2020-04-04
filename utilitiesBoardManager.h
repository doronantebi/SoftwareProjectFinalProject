
#ifndef SOFTWAREPROJECTFINALPROJECT_UTILITIESBOARDMANAGER_H
#define SOFTWAREPROJECTFINALPROJECT_UTILITIESBOARDMANAGER_H

#include "utilitiesLinkedList.h"

enum Mode {
    Init = 0,
    Edit = 1,
    Solve = 2
};

struct sudokuManager {
    int n;
    int m;
    int *board;
    int *fixed;
    int *erroneous;
    struct movesList *linkedList;
    int addMarks;
};




/* GENERAL METHODS */
/*
 * This function calculates the row's lower bound of the block.
 */
int rowLowBound(int m, int i);

/*
 * This function calculates the row's higher bound of the block.
 */
int rowHighBound(int m, int i);

/*
 * This function calculates the column's lower bound of the block.
 */
int colLowBound(int n, int j);

/*
 * This function calculates the column's higher bound of the block.
 */
int colHighBound(int n, int j);

/*
 * This method returns the length of the sudoku board
 */
int boardLen(struct sudokuManager *manager);

/*
 * returns the amount of cells in the board
 */
int boardArea(struct sudokuManager *manager);

/*
 * This method returns the matrix index in the array of the board
 */
int matIndex(int m, int n, int i, int j);

/*
 * This method returns if a cell is a fixed cell,
 * if it is, returns 1,
 * otherwise, returns 0.
 */
int isFixedCell(struct sudokuManager *manager, int row, int col);

/*
 * Returns the amount of times that the row contains val.
 */
int rowContains(int *board, int m, int n, int i, int val);

/*
 * Returns the amount of times that the column contains val
 */
int colContains(int *board, int m, int n, int j, int val);


/*
 * Returns the amount of times that the block of (i,j)
 * contains val.
 */
int blockContains(int *board, int m, int n, int i, int j, int val);

/*
 * This method returns 1 if a row/col/block contains
 * val more than once.
 */
int neighbourContains(int *board, int m, int n, int i, int j, int val);

/*
 * This method returns if the value of the cell is not legal,
 * if it is, returns 1,
 * otherwise, returns 0.
 */
int isErroneous(struct sudokuManager *manager, int i, int j);


/*
 * This function returns True if 0 <= x <= n*m
 */
int isLegalCellValue(struct sudokuManager *manager, int x);

/*
* counts the empty cells in the board
*/
int amountOfEmptyCells(struct sudokuManager *manager);

/*
 * This updates the erroneous board for a sudokuBoard
 * input is a pointer to the board typed *int.
 * Will be used when Loading a file in Edit mode
 * or when loading a file in Solve mode when setting addMarks to be 1
 * UPDATES THE WHOLE BOARD
 * will be used when scanning a file.
 * can be used for the original board or for the fixed board.
 */
int updateErroneousBoard(int* board, int* erroneous, int m, int n);

/*
 * This function updates the erronous board of a sudokuManager.
* it will update all row i, column j, and the block that (i,j) is in.
 * will be used in functions like set.
*/
int updateErroneousBoardCell(int* board, int* erroneous, int m, int n, int i, int j);


/*
 * This method frees a given board.
 */
void freeBoard(struct sudokuManager *board);

/*
 * This method sets manager->board[row][col] = val
 */
void changeCellValue(int *board, int m, int n, int row, int col, int val);

#endif