
#ifndef SOFTWAREPROJECTFINALPROJECT_GAME_H
#define SOFTWAREPROJECTFINALPROJECT_GAME_H

enum Mode {
    Init = 0,
    Edit = 1,
    Solve = 2
};

typedef struct {
    int n;
    int m;
    int *board;
    int *fixed;
    struct movesList *linked_list;
    enum mode;
    int addMarks;  /*1 if we need to mark erroneous cells, 0 otherwise*/
}sudokuManager;

static enum Mode mode = Init;

/*
 * This method is used for starting to play sudoku.
 */
void startGame();

/*
 * This function uploads a file of a game to solve.
 */
struct sudokuManager* solve(char *fileName);

/*
 * This function uploads a file of a game to edit.
 */
struct sudokuManager* edit(char *fileName);

/*
 * Sets the "mark errors" setting to X, where X is 0 or 1.
 */
void markErrors(struct sudokuManager *board, int X);

void printBoard(struct sudokoManager *board);

void changeMode(enum Mode newMode);

#endif