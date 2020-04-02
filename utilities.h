#ifndef SOFTWAREPROJECTFINALPROJECT_UTILITIES_H
#define SOFTWAREPROJECTFINALPROJECT_UTILITIES_H


#endif //SOFTWAREPROJECTFINALPROJECT_UTILITIES_H

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

enum action{
    startCommand = 0,
    command = 1,
    finishCommand = 2
};

struct movesList{
    struct sudokuManager **board;
    struct movesList *next;
    struct movesList *prev;
    int row;
    int col;/* */
    int prevValue;
    int newValue;
    enum action action;
};


/* GENERAL METHODS */
/*
 * This function calculates the row's lower bound of the block.
 */
int rowLowBound(struct sudokuManager *manager, int i);

/*
 * This function calculates the row's higher bound of the block.
 */
int rowHighBound(struct sudokuManager *manager, int i);

/*
 * This function calculates the column's lower bound of the block.
 */
int colLowBound(struct sudokuManager *manager, int j);

/*
 * This function calculates the column's higher bound of the block.
 */
int colHighBound(struct sudokuManager *manager, int j);

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
int matIndex(struct sudokuManager *manager, int i, int j);

/*
 * This method returns if a cell is a fixed cell,
 * if it is, returns 1,
 * otherwise, returns 0.
 */
int isFixedCell(struct sudokuManager *manager, int row, int col);

/*
 * Returns the amount of times that the row contains val.
 */
int rowContains(struct sudokuManager *manager, int i, int val);

/*
 * Returns the amount of times that the column contains val
 */
int colContains(struct sudokuManager *manager, int j, int val);


/*
 * Returns the amount of times that the block of (i,j)
 * contains val.
 */
int blockContains(struct sudokuManager *manager, int i, int j, int val);

/*
 * This method returns 1 if a row/col/block contains
 * val more than once.
 */
int neighbourContains(struct sudokuManager *manager, int i, int j, int val);

/*
 * This method returns if the value of the cell is legal,
 * if it is, returns 1,
 * otherwise, returns 0.
 */
int isLegalCell(struct sudokuManager *manager, int i, int j);

/*
 * This function returns True if 1 <= x <= n*m
 */
int isLegalValue(struct sudokuManager *manager, int x);

/*
* counts the empty cells in the board
*/
int amountOfEmptyCells(struct sudokuManager *manager);