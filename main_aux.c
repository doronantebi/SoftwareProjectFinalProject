#include <stdio.h>
#include "main_aux.h"

#include "game.h"
#define title "                              ,,                               \n .M\"\"\"bgd                   `7MM              `7MM                  \n,MI    \"Y                     MM                MM                  \n`MMb.   `  7MM  `7MM     ,M\"\"bMM    ,pW\"Wq.     MM  ,MP'  `7MM  `7MM  \n  `YMMNq.   MM    MM   ,AP    MM   6W'   `Wb    MM ;Y       MM    MM  \n.     `MM   MM    MM   8MI    MM   8M     M8    MM;Mm       MM    MM  \nMb     dM   MM    MM   `Mb    MM   YA.   ,A9    MM `Mb.     MM    MM  \nP\"Ybmmd\"    `Mbod\"YML.  `Wbmd\"MML.  `Ybmd9'.   JMML. YA.    `Mbod\"YML."
#define creators "Din Moshe and Doron Antebi"

void printGameTitle(){
    printf("%s\n\n             Creators: %s.\n", title, creators);
}



void printNoNextMoveError(){
    printf("There is no next move\n");
}

void printErrorAutofill(){
    printf("Autofill error!\n");
}

/* PRINT RELATED METHODS */
/*
 * length should be equal to 4N+m+1
 */
void printSeperatorRow(int length){
    int i;
    for (i = 0; i < length ; i++) {
        printf("-");
    }
    printf("\n");
}

/*
 *
 */
void printCellRow(){
    printf("|");
}

/*
 * This method prints the cell (row, col)
 */
void printCell(struct sudokuManager *manager, int row, int col){
    int* puzzle = manager->board;
    int addErrors = 0;
    int index;
    if (manager->addMarks == 1 || mode == Edit){
        addErrors = 1;
    }
    index = matIndex(manager,row,col);
    if(isFixedCell(manager, row, col)){
        printf("%2d.",puzzle[index]);
        return;
    }
    if(!isLegalCell(manager, row, col)&&(addErrors)){
        printf("%2d*",puzzle[index]);
    } else {
        if(puzzle[index] == 0){
            printf("  ");
        } else{
            printf("%2d",puzzle[index]);
        }
    }

}

void printSudokuGrid(struct sudokuManager *manager) {
    int n = manager->n, m = manager->m;
    int N = boardLen(manager) ;
    int i, j, k;
    for (i = 0; i < m; i++) {
        printSeperatorRow(4*N + m + 1);
        for (j = 0; j < n; j++){
            printCellRow();
            for (k = 0; k < m ; k++ ){
                printCell(manager, i*n + j, j*m + k);
            }
            printSeperatorRow(4*N + m + 1);
        }
    }
}

