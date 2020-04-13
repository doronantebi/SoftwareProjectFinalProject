
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
 * Returns the amount of times that the column contains val
 */
int colContains(int *board, int m, int n, int col, int val);


/*
 * Returns the amount of times that the block of (row,column)
 * contains value
 */
int blockContains(int *board, int m, int n, int row, int col, int val);

/*
 * This method returns 1 if the row, column or relevant block contains
 * given value more than once.
 */
int neighbourContains(int *board, int m, int n, int row, int col, int val);



/*
 * This method returns 1 if a row/col/block contains
 * val once.
 */
int neighbourContainsOnce(int* board, int m, int n, int i, int j, int val);

/*
 * This method returns 1 if a row/col/block contains
 * val more than once.
 */
int neighbourContainsTwice(int* board, int m, int n, int row, int col, int val);


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
 * This method checks if the current cell in last is row, assuming length of row is size.
 */
int isLastInRow(int size, int j);


/*
* This method checks if the current cell is last in the matrix, assuming the matrix is size X size.
*/
int isLastCellInMatrix(int size, int i, int j);


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
 * the function returns 1 if one of the cells is illegal
 * and 0 otherwise.
 */
int updateErroneousBoard(int* board, int* erroneous, int m, int n);

/*
 * This function updates the erronous board of a sudokuManager.
* it will update all row i, column j, and the block that (i,j) is in.
 * will be used in functions like set.
 * the function returns 1 if one of the cells is illegal
 * and 0 otherwise.
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


/*
 * This method fills onlyFixed of integers containing only the fixed values in board.
 * If there is an error allocating memory, it returns NULL and prints a message to the user.
 */
int *copyFixedOnly(struct sudokuManager *board, int *onlyFixed);

/*
 * This method copies currentGrid to retGrid.
 */
void duplicateBoard(int* currentGrid, int *retGrid, int m, int n);


/*
 * if one of the cells in the errouneous board is 1, returns 1.
 */
int isAnyErroneousCell(struct sudokuManager *manager);


/*
 * This function updates the board with autofilled values
 */
int updateAutofillValuesBoard(struct sudokuManager *board);


/*
 * This method assigns the first cell indices of the block numbered blockNum into (pRow, pCol).
 * 0 <= blockNum < n*m
 */
void getFirstIndexInBlock(int m, int n, int blockNum, int* pRow, int* pCol);



/*
 * this method sets Z to (row,col),
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
 * set all pointers in board to NULL
 * and all integers to -1
 */
void initNullBoard(struct sudokuManager *manager);

/*
* This function updated the board values by given parameters
*/
void initBoardValues(struct sudokuManager *boardToFill, int m, int n, int *board, int *erroneous, int *fixed, int addMarks, int emptyCells,
                     struct movesList *list);

#endif
