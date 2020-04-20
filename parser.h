
/*
 * This module is meant to handle the parsing of commands received from the user.
 * It receives the commands from the game.c module and interprets them to see if they are valid,
 * have the right number of parameters and that the parameters are in the correct range.
 * If the command is valid, it runs the matching command by calling to the right function from the game.c module.
 * If it is not valid, it outputs an informative message to the user, saying what ought to be fixed.
 */

#ifndef SOFTWAREPROJECTFINALPROJECT_PARSER_H
#define SOFTWAREPROJECTFINALPROJECT_PARSER_H


#include <stdlib.h>
#include <stdio.h>
#include "util/board_manager.h"
#include "main_aux.h"
#include "game.h"

/*
 * The function reads a command line and interprets it.
 * The function then executes the proper command by calling to the command in Game.
 */
int interpret(char *command, struct sudokuManager **board, enum Mode mode);

#endif
