
#include "gurobi_sys"
#include "utilitiesBoardManager.h"
#include "main_aux.h"

#include "game.h"
#include "solver.h"

#ifndef SOFTWAREPROJECTFINALPROJECT_GUROBI_H
#define SOFTWAREPROJECTFINALPROJECT_GUROBI_H


typedef enum {
    BINARY = 0,
    INTEGER = 1,
    CONTINUOUS = 2
} GurobiOption;


int solveGurobi(struct sudokuManager *manager, GurobiOption type, int** retBoard);


#endif
