
#include "Parser.h"
#include <stdio.h>
#include <string.h>
#include "game.h"
#include "main_aux.h"
#include "utilitiesBoardManager.h"

#define commandListLength 17

/*
 * This method assumes the command entered is solve,
 * checks the validity of the rest of the command and executes it.
 * Available in every mode.
 * It returns
 */
int interpretSolve(char *token, struct sudokuManager *board){

    token = strtok(NULL, " \t\r\n");

    if(token == NULL){ /* Not enough parameters*/
        printFewParams(1, 0);
        return 0;
    }
    else{
        if (strtok(NULL, " \t\r\n") != NULL){ /*Too many parameters*/
           printExtraParams(1, 0);
           return 0;
        }
        else{
            return solve(board, token);
        }
    }
}

/*
 * This method assumes the command entered is edit,
 * checks the validity of the rest of the command and executes it.
 * Available in every mode.
 * It returns
 */
int interpretEdit(char *token, struct sudokuManager *board){
    int arrNumOfParams[2];

    token = strtok(NULL, " \t\r\n");

    if (strtok(NULL, " \t\r\n") != NULL){ /*Too many parameters*/
        arrNumOfParams = {0, 1};
        printExtraParamsExtend(arrNumOfParams, 2, 1);
        return 0;
    }
    else{
        return edit(board, token); /* Sending Null in token if no parameters were entered. */
    }
}

/*
 * This method assumes the command entered is mark_errors,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve mode only.
 * It returns
 */
int interpretMarkErrors(char *token, struct sudokuManager *board, enum Mode mode){
    enum Mode availableModes[1] = {Solve};
    int input, check;
    if (mode != Solve){
        printUnavailableMode(2, mode, availableModes, 1);
        return 0;
    }
    token = strtok(NULL, " \t\r\n");

    if(token == NULL){
        printFewParams(1, 2);
        return 0;
    }
    else{
        if (strtok(NULL, " \t\r\n") != NULL){ /*Too many parameters*/
            printExtraParams(1, 2);
            return 0;
        }
        else{
            check = sscanf(token, "%d", &input);
            if (check == 1){
                if (input == 0 || input == 1){
                    markErrors(board, input);
                    return 0;
                }
                else{
                    printWrongRangeInt(2, input, 1);
                    printf("The parameter should be only 1 or 0\n");
                    return 0;
                }
            }
            printNotANumber(1);
            return 0;
        }
    }
}

/*
 * This method receives a token - the name of the command, the current mode, the index of the command,
 * its available modes and the length of availableModes.
 * If the current mode is unavailable, it returns -1.
 * If there are any other arguments it also returns -1.
 * Otherwise, it returns 0.
 */
int interpretNoArguments(char *token, enum Mode mode, int indexCommand, enum Mode *availableModes, int len){
    int i, isAvailable = 0;

    for (i = 0; i < len; i++){
        if (availableModes[i] == mode){
            isAvailable = 1;
        }
    }

    if (isAvailable == 0){
        printUnavailableMode(indexCommand, mode, availableModes, len);
        return -1;
    }

    token = strtok(NULL, " \t\r\n");

    if (token != NULL){ /* Too many parameters */
        printExtraParams(0, indexCommand);
        return -1;
    }
    return 0;
}

/*
 * This method assumes the command entered is print_board,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns 0.
 */
int interpretPrintBoard(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};

    if (interpretNoArguments(token, mode, 3, availableModes, 2) == -1){
        return 0;
    }
    else{
        printBoard(board);
        return 0;
    }
}


/*
 * This method assumes the command entered is set, checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns 0.
 */
int interpretSet(char *token, struct sudokuManager *board, enum Mode mode){
    int i = 0;
    int arrInput[3], arrCheck[3];  /*arrCheck saves whether we succeeded in converting
                                    * the string into a number for every parameter*/
    int len;
    enum Mode availableModes[2] = {Solve, Edit};
    if (mode == Init) {
        printUnavailableMode(4, mode, availableModes, 2);
        return 0;
    }
    while ((token = strtok(NULL, " \t\r\n")) != NULL && i < 3){
        arrCheck[i] = sscanf(token, "%d", &arrInput[i]);
        i++;
    }
    if (i < 3){ /* Not enough parameters*/
        printFewParams(3, 4);
        return 0;
    }
    else {
        if (strtok(NULL, " \t\r\n") != NULL) { /*Too many parameters*/
            printExtraParams(3, 4);
            return 0;
        } else {
            len = boardLen(board);
            if (arrCheck[0] != 1) {
                printNotANumber(1);
                return 0;
            } else {
                if (!(arrInput[0] - 1 >= 0 && arrInput[0] - 1 < len)) {
                    printWrongRangeInt(4, arrInput[0], 1);
                    printf("The parameter should be an integer between 0 and %d.\n", len - 1);
                    return 0;
                } else {
                    if (arrCheck[1] != 1) {
                        printNotANumber(2);
                        return 0;
                    } else {
                        if (!(arrInput[1] - 1 >= 0 && arrInput[1] - 1 < len)) {
                            printWrongRangeInt(4, arrInput[1], 2);
                            printf("The parameter should be an integer between 0 and %d.\n", len - 1);
                            return 0;
                        } else {
                            if (arrCheck[2] != 1) {
                                printNotANumber(2);
                                return 0;
                            } else {
                                if (!(isLegalCellValue(arrInput[2]))) {
                                    printWrongRangeInt(4, arrInput[2], 3);
                                    printf("The parameter should be an integer between 0 and %d.\n", len);
                                    return 0;
                                } else { /* All parameters are valid */
                                    return set(board, arrInput[0], arrInput[1], arrInput[2]);
                                }
                            }
                        }
                    }
                }
            }
        }

    }
}


/*
 * This method assumes the command entered is validate,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns
 */
int interpretValidate(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};
    if (interpretNoArguments(token, mode, 5, availableModes, 2) == -1){
        return 0;
    }
    else{
        return validate(board);
    }
}

/*
 * This method assumes the command entered is guess,
 * checks the validity of the rest of the command and executes it.
 * Available only in Solve mode.
 * It returns
 */
int interpretGuess(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[1] = {Solve};
    int check;
    float input;

    if (mode != Solve) {
        printUnavailableMode(6, mode, availableModes, 1);
        return 0;
    }
    token = strtok(NULL, " \t\r\n");

    if (token == NULL){ /* Not enough parameters */
        printFewParams(1, 6);
        return 0;
    }
    else{
        if (strtok(NULL, " \t\r\n") != NULL){ /*Too many parameters*/
            printExtraParams(1, 6);
            return 0;
        }
        else{
            check = sscanf(token, "%f", &input);
            if (check != 1){ /* Input is not a float*/
                printNotAFloat(1);
                return 0;
            }
            else{
                if (!(input >=0 && input <= 1)){ /*Not in the correct range*/
                    printWrongRangeFloat(6, input, 1);
                    printf("The parameter should be a float between 0 and 1.\n");
                    return 0;
                }
                else{
                    return guess(board, input);
                }
            }
        }
    }
}

/*
 * This method assumes the command entered is generate,
 * checks the validity of the rest of the command and executes it.
 * Available only in Edit mode.
 * It returns
 */
int interpretGenerate(char *token, struct sudokuManager *board, enum Mode mode) {
    int i = 0, emptyCells;
    enum Mode availableModes[1] = {Edit};
    int arrInput[2], arrCheck[2];  /*arrCheck saves whether we succeeded in converting
                                    * the string into a number for every parameter*/

    if (mode != Edit) {
        printUnavailableMode(7, mode, availableModes, 1);
        return 0;
    }

    while ((token = strtok(NULL, " \t\r\n")) != NULL && i < 2) {
        arrCheck[i] = sscanf(token, "%d", &arrInput[i]);
        i++;
    }

    if (i < 2) { /* Not enough parameters*/
        printFewParams(2, 7);
        return 0;
    } else {
        if (strtok(NULL, " \t\r\n") != NULL) { /*Too many parameters*/
            printExtraParams(2, 7);
            return 0;
        } else {
            if (arrCheck[0] != 1) {
                printNotANumber(1);
                return 0;
            } else {
                if (!(arrInput[0] >= 0)) {
                    printWrongRangeInt(7, arrInput[0], 1);
                    printf("The parameter should be an integer greater than or equal to 0.\n");
                    return 0;
                } else {
                    emptyCells = amountOfEmptyCells(board);
                    if (emptyCells < arrInput[0]) { /* checking for number of empty cells */
                        printNotEnoughEmptyCells(emptyCells, arrInput[0]);
                        return 0;
                    } else {
                        if (arrCheck[1] != 1) {
                            printNotANumber(2);
                            return 0;
                        } else {
                            if (!(arrInput[1] > 0)) {
                                printWrongRangeInt(7, arrInput[1], 2);
                                printf("The parameter should be an integer greater than 0.\n");
                                return 0;
                            } else { /* All parameters are valid */
                                return generate(board, arrInput[0], arrInput[1]);
                            }
                        }
                    }
                }
            }
        }
    }
}

/*
 * This method assumes the command entered is undo,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns
 */
int interpretUndo(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};

    if (interpretNoArguments(token, mode, 8, availableModes, 2) == -1){
        return 0;
    }
    else{
        return undo(board);
    }
}


/*
 * This method assumes the command entered is redo,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns
 */
int interpretRedo(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};
    if (interpretNoArguments(token, mode, 9, availableModes, 2) == -1){
        return 0;
    }
    else{
        return redo(board);
    }
}

/*
 * This method assumes the command entered is save,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns
 */
int interpretSave(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};
    if (mode == Init) {
        printUnavailableMode(9, mode, availableModes, 2);
        return 0;
    }
    token = strtok(NULL, " \t\r\n");

    if(token == NULL){ /* Not enough parameters*/
        printFewParams(1, 10);
        return 0;
    }
    else{
        if (strtok(NULL, " \t\r\n") != NULL){ /*Too many parameters*/
            printExtraParams(1, 10);
            return 0;
        }
        else{
            return save(board, token);
        }
    }
}



/*
 * This method assumes the command entered is hint or guess_hint,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve mode only.
 * It receives a boolean parameter saying if it is hint or guess_hint.
 * It returns 0.
 */
int interpretHintOrGuessHint(char *token, struct sudokuManager *board, int isHint, enum Mode mode){
    int i = 0;
    int arrInput[2], arrCheck[2];  /*arrCheck saves whether we succeeded in converting
                                    * the string into a number for every parameter*/
    int len;
    enum Mode availableModes[1] = {Solve};
    if (mode != Solve) {
        printUnavailableMode(11, mode, availableModes, 1);
        return 0;
    }
    while ((token = strtok(NULL, " \t\r\n")) != NULL && i < 2){
        arrCheck[i] = sscanf(token, "%d", &arrInput[i]);
        i++;
    }
    if (i < 2){ /* Not enough parameters*/
        printFewParams(2, 11);
        return 0;
    }
    else {
        if (strtok(NULL, " \t\r\n") != NULL) { /*Too many parameters*/
            printExtraParams(2, 11);
            return 0;
        } else {
            len = boardLen(board);
            if (arrCheck[0] != 1) {
                printNotANumber(1);
                return 0;
            } else {
                if (!(arrInput[0] - 1 >= 0 && arrInput[0] - 1 < len)) {
                    printWrongRangeInt(11, arrInput[0], 1);
                    printf("The parameter should be an integer between 0 and %d.\n", len - 1);
                    return 0;
                } else {
                    if (arrCheck[1] != 1) {
                        printNotANumber(2);
                        return 0;
                    } else {
                        if (!(arrInput[1] - 1 >= 0 && arrInput[1] - 1 < len)) {
                            printWrongRangeInt(11, arrInput[1], 2);
                            printf("The parameter should be an integer between 0 and %d.\n", len - 1);
                            return 0;
                        } else { /* All parameters are valid */
                            if (isHint){
                                return hint(arrInput[0], arrInput[1], board);
                            }
                            else{
                                return guessHint(arrInput[0], arrInput[1], board);
                            }
                        }
                    }
                }
            }
        }
    }
}


/*
 * This method assumes the command entered is num_solutions,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns
 */
int interpretNumSolutions(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};
    if (interpretNoArguments(token, mode, 13, availableModes, 2) == -1){
        return 0;
    }
    else{
        numSolutions(board);
        return 0;
    }
}

/*
 * This method assumes the command entered is autofill,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns
 */
int interpretAutofill(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve};
    if (interpretNoArguments(token, mode, 14, availableModes, 1) == -1){
        return 0;
    }
    else{
        autofill(board);
        return 0;
    }
}

/*
 * This method assumes the command entered is reset,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns
 */
int interpretNumSolutions(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};
    if (interpretNoArguments(token, mode, 15, availableModes, 2) == -1){
        return 0;
    }
    else{
        reset(board);
        return 0;
    }
}

/*
 * This method assumes the command entered is reset,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns
 */
int interpretNumSolutions(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Init, Solve, Edit};
    if (interpretNoArguments(token, mode, 16, availableModes, 3) == -1){
        return 0;
    }
    else{
        reset(board);
        return 0;
    }
}
/*
 * This method receives a word and returns a number unique to the
 * command if it is a legal command, or -1 if it is an invalid command.
 * It returns the index of the command in the array commandList.
 */
int commandNum (char* word){
    int i;
    for (i = 0; i < commandListLength; i++){
        if (strcmp(word, commandList[i]) == 0) { /* strings are equal*/
            return i;
        }
    }
    return -1;
}

/*
 * This method interprets the command the user entered and
 * executes it by calling to the proper method in Game.
 * It returns -1 if we need to terminate. Otherwise, it returns 0.
 * If exit command has been received, it returns 2.
 */
int interpret(char *command, struct sudokuManager **pBoard){
    char *token;
    struct sudokuManager *board = *pBoard;
    int index;

    if (strlen(command) == 257){
        printf("Error: too many characters in a single line.\n"
               "A line should contain up to 256 characters.\nPlease enter a new command.\n");
        return 0;
    }
    token = strtok(command, " \t\r\n");
    if (token != NULL) { /* First token exists*/

        index = commandNum(token);

        switch (index){
            case 0:
                return interpretSolve(token, board);
            case 1:
                return interpretEdit(token, board);
            case 2:
                return interpretMarkErrors(token, board);
            case 3:
                return interpretPrintBoard(token, board);
            case 4:
                return interpretSet(token, board);
            case 5:
                return interpretValidate(token, board);
            case 6:
                return interpretGuess(token, board);
            case 7:
                return interpretGenerate(token, board);
            case 8:
                return interpretUndo(token, board);
            case 9:
                return interpretRedo(token, board);
            case 10:
                return interpretSave(token, board);
            case 11:
                return interpretHintOrGuessHint(token, board, 1);
            case 12:
                return interpretHintOrGuessHint(token, board, 0);
            case 13:
                return interpretNumSolutions(token, board);
            case 14:
                return interpretAutofill(token, board);
            case 15:
                return interpretReset(token, board);
            case 16:
                return interpretExit(token, board);
            case -1:
                printInvalidCommand();
                printf("The command entered does not exist. Please enter a new command.\n");
                return 0;
        }

    }

    return 0;
}


