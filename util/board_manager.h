/*
 * This module deals with everything that has to do with the sudokuManager struct.
 * It have methods regarding this struct which can be used by any module which includes utilitiesBoardManager.c
 * and uses sudokuManager struct.
 */

#ifndef SOFTWAREPROJECTFINALPROJECT_BOARD_MANAGER_H
#define SOFTWAREPROJECTFINALPROJECT_BOARD_MANAGER_H

#include "linked_list.h"

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
    int emptyCells;
};

/* GENERAL METHODS */

/*
 * This function calculates the row's lower bound of the block.
 */
int rowLowBound(int m, int row);

/*
 * This function calculates the row's higher bound of the block.
 */
int rowHighBound(int m, int row);

/*
 * This function calculates the column's lower bound of the block.
 */
int colLowBound(int n, int column);

/*
 * This function calculates the column's higher bound of the block.
 */
int colHighBound(int n, int column);

/*
 * This method returns the length of the sudoku board.
 */
int boardLen(struct sudokuManager *manager);

/*
 * returns the amount of cells in the board.
 */
int boardArea(struct sudokuManager *manager);

/*
 * This method returns the matrix index in the array of the board.
 */
int matIndex(int m, int n, int row, int col);

/*
 * This method returns if a cell is a fixed cell,
 * if it is, returns 1,
 * otherwise, returns 0.
 */
int isFixedCell(struct sudokuManager *manager, int row, int column);

/*
 * Returns the amount of times that the row contains val.
 */
int rowContains(int *board, int m, int n, int row, int val);

/*
 * Returns the amount of times that the column contains val.
 */
int colContains(int *board, int m, int n, int col, int val);

/*
 * Returns the amount of times that the block of <row,col> contains val.
 */
int blockContains(int *board, int m, int n, int row, int col, int val);

/*
 * This method returns 1 if a row/col/block contains val at least once.
 */
int neighbourContainsOnce(int* board, int m, int n, int i, int j, int val);

/*
 * This method returns 1 if a row/col/block contains val more than once.
 */
int neighbourContainsTwice(int* board, int m, int n, int row, int col, int val);

/*
 * This method returns if the value of the cell <i,j> is not legal,
 * if it is, returns 1, otherwise, returns 0.
 */
int isErroneous(struct sudokuManager *manager, int i, int j);

/*
 * This method checks that the value x is in the correct range for the current board.
 */
int isLegalCellValue(struct sudokuManager *manager, int x);

/*
 * This method checks if the current cell in last is row, assuming length of row is size.
 */
int isLastInRow(int size, int j);

/*
* This method checks if the current cell is last in the matrix, assuming the matrix is size X size.
*/
int isLastCellInMatrix(int size, int i, int j);

/*
 * This method counts the empty cells in the board.
 */
int amountOfEmptyCells(struct sudokuManager *manager);

/*
 * This updates the erroneous board for a sudokuBoard.
 * input is a pointer to the board typed *int.
 * Will be used when Loading a file in "edit" or "solve" functions.
 * the function returns 1 if one of the cells is erroneous, and 0 otherwise.
 */
int updateErroneousBoard(int* board, int* erroneous, int m, int n);

/*
 * This function updates the erroneous board of a sudokuManager.
 * it will update all row, column, and the block that <row, col> is in.
 * the function returns 1 if one of the cells is illegal and 0 otherwise.
 */
int updateErroneousBoardCell(int* board, int* erroneous, int m, int n, int row, int col);

/*
 * This method frees a given board.
 */
void freeBoard(struct sudokuManager *board);

/*
 * This method sets manager->board[row][col] = val
 */
void changeCellValue(int *board, int m, int n, int row, int col, int val);

/*
 * This method fills onlyFixed of integers containing only the fixed values in board.
 * If there is an error allocating memory, it returns NULL and prints a message to the user.
 */
int *copyFixedOnly(struct sudokuManager *board, int *onlyFixed);

/*
* This method copies array from to array to.
*/
void duplicateBoard(int* from, int *to, int m, int n);

/*
 * This function checks if the board is erroneous.
 * If one of the cells is erroneous it returns 1, Otherwise, it returns 0.
 */
int isAnyErroneousCell(struct sudokuManager *manager);

/*
 * This method updates the board with autofilled values.
 */
int updateAutofillValuesBoard(struct sudokuManager *board);

/*
 * This method assigns the first cell indices of the block numbered blockNum into (pRow, pCol).
 * 0 <= blockNum < n*m
 */
void getFirstIndexInBlock(int m, int n, int blockNum, int* pRow, int* pCol);

/*
 * this method sets Z to <row, col>,
 * is called after verifying that all values are legal.
 */
int doSet(struct sudokuManager *manager, int row, int col, int Z);

/*
 * This function updates the epmtyCells field after a single set.
 */
void updateEmptyCellsSingleSet(struct sudokuManager *manager, int prevVal, int nextVal);

/*
 * This function updates the enptyCells field by counting the empty cells.
 */
void updateEmptyCellsField(struct sudokuManager *manager);

/*
 * This method sets all pointers in board to NULL and all integers to -1.
 */
void initNullBoard(struct sudokuManager *manager);

/*
* This function updated the board values by given parameters.
*/
void initBoardValues(struct sudokuManager *boardToFill, int m, int n, int *board, int *erroneous, int *fixed,
        int emptyCells, struct movesList *list);

/*
 * This function receives a board, and index.
 * if cell <row, col> has only one legal value to fill, it returns it.
 * if there is no legal value, it returns -1.
 * if there is more than one legal value, it returns 0.
 */
int returnLegalValue(int* board, int m, int n, int row, int col);

#endif
