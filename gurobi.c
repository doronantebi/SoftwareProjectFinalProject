#include "gurobi.h"


/* Copyright 2013, Gurobi Optimization, Inc. */

/* This example formulates and solves the following simple MIP model:

     maximize    x + 3 y + 2 z
     subject to  x + 2 y + 3 z <= 5
                 x +   y       >= 1
     x, y, z binary
*/

#include <stdlib.h>
#include <stdio.h>
#include "gurobi_sys"
#include "utilitiesBoardManager.h"
#include "main_aux.h"
#include <time.h>



typedef enum {
    BINARY = 0,
    INTEGER = 1,
    CONTINUOUS = 2
} GurobiOption;

/*
 *
 */
typedef struct {
    int row; /* row in board */
    int col; /* column in board */
    int val;  /* matching value */
} Variable ;

/* generate a random floating point number from min to max */
double randRange(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}


/*
 * This function gets a sudokuManager, and 3 dimensional array initialized with zeros.
 * will update the ***indices such that a cell will contain -1 if we don't want it to become a variable,
 * and with its variable index.
 * the function returns the amount of variables we need in our gurobi program.
 */
int update3DIndices(struct sudokuManager *manager, Variable **indices){
    int length = boardLen(manager);
    int* board = manager->board;
    int row, col, m = manager->m, n = manager->n, val = 0;
    int i, j, height;
    int blockRowLowBound, blockRowHighBound, blockColLowBound, blockColHighBound;
    int count = 0;
    for (row = 0; row < length; row++) {
        for(col = 0; col < length; col++){
            val = board[matIndex(m,n,row,col)];
            if(val == 0){
                continue;
            }
            /* update for all values for this cell */
            for(height = 0; height < length; height++){
                indices[row][col][height] = -1; /* */
            }
            /* update for all row */
            for(j = 0; j < length; j++){
                indices[row][j][val] = -1;
            }
            /* update for all column */
            for(i = 0; i < length; i++){
                indices[i][col][val] = -1;
            }
            /* update for all block */
            blockRowLowBound = rowLowBound(m, row);
            blockRowHighBound = rowHighBound(m, row);
            blockColLowBound = colLowBound(n, col);
            blockColHighBound = colHighBound(n, col);
            for(i = blockRowLowBound ; i < blockRowHighBound ; i++){
                for(j = blockColLowBound ; j < blockColHighBound ; j++) {
                    indices[i][j][val] = -1;
                }
            }
        }
    }
    for(row = 0; row < length; row++){
        for(col = 0; col<length; col++){
            for (height = 0; height < length ;height ++) {
                if(indices[row][col][height] != -1){ /* only if we want to have this variable */
                indices[row][col][height] = count; /* update in the relevant cell its variable index number  */
                count ++;
                }
            }
        }
    }
    return count; /* returns the amount of variables */
}



/*
 * Initializes a 3D array with zeros.
 */
int *** init3DArray(int length){
    int i, j;
    int ***array = (int ***)malloc(length*sizeof(int**));
    if(array == NULL){
        printAllocFailed();
        return NULL;
    }

    for (i = 0; i < length ; i++) {
        array[i] = (int**)malloc(length* sizeof(int*));
        if(array[i]==NULL){
            printAllocFailed();
            return NULL;
        }
        for (j = 0; j < length; j++) {
            array[i][j] = (int*)calloc(length, sizeof(int));
            if(array[i][j] == NULL){
                printAllocFailed();
                return NULL;
            }
        }
    }
    return array;
}

/*
 * This method frees the 3D array.
 */
void free3DArray(int ***array, int length){
    int i, j;
    for(i = 0; i < length; i++){
        for(j = 0; j < length; j++){
            free(array[i][j]);
        }
    }
    for(i = 0 ; i < length; i++){
        free(array[i]);
    }
    free(array);
}


/*
 * This function updates vtype array values by the given enum of Gurobi Option
 */
void initVariableType(GurobiOption type, char *vtype, int amountOfVariables) {
    int i;
    switch(type){
        case BINARY:
            for (i = 0; i < amountOfVariables; i++) {
                vtype[i] = GRB_BINARY;

            }
            break;
        case CONTINUOUS:
            for (i = 0; i < amountOfVariables; i++) {
                vtype[i] = GRB_CONTINUOUS;
            }
            break;
        case INTEGER:
            for (i = 0; i < amountOfVariables; i++) {
                vtype[i] = GRB_INTEGER;
            }
            break;
    }
}

/*
 * This method returns the size of the constraint builds on a row
 */
int getConstaintRowLength(int val, int row, struct sudokuManager *manager, int ***indices){
    int length = boardLen(manager);
    int col, count = 0;
    for(col = 0; col < length; col++){
        if(indices[row][col][val] == -1){
            continue;
        }
        count ++;
    }
    return count;
}

/*
 * This function builds an array of a constraint of a row by a given length.
 */
int* getConstraintRow(int val, int row, struct sudokuManager *manager, int ***indices, int constraintLen){
    int length = boardLen(manager);
    int col, count = 0;
    int *constraint;
    constraint = malloc(constraintLen* sizeof(int));
    if(constraint == NULL) {
        printAllocFailed();
        return NULL;
    }
    for(col = 0; col < length; col++){
        if(indices[row][col][val] == -1){
            continue;
        }
        constraint[count] = indices[row][col][val];
        count++;
    }
    return constraint;
}

/*
 * This method returns the size of the constraint built on a col
 */
int getConstaintColLength(int val, int col, struct sudokuManager *manager, int ***indices){
    int length = boardLen(manager);
    int row, count = 0;
    for(row = 0; row < length; row++){
        if(indices[row][col][val] == -1){
            continue;
        }
        count ++;
    }
    return count;
}


/*
 * This function builds an array of a constraint of a col by a given length.
 */
int* getConstraintCol(int val, int col, struct sudokuManager *manager, int ***indices, int constraintLen){
    int length = boardLen(manager);
    int row, count = 0;
    int *constraint;
    constraint = malloc(constraintLen* sizeof(int));
    if(constraint == NULL) {
        printAllocFailed();
        return NULL;
    }
    for(row = 0; row < length; row++){
        if(indices[row][col][val] == -1){
            continue;
        }
        constraint[count] = indices[row][col][val];
        count++;
    }
    return constraint;
}


/*
 * This method returns the size of the constraint built on a block
 */
int getConstraintBlockLength(int val, int row, int col, struct sudokuManager *manager, int ***indices){
    int m = manager->m, n = manager->n;
    int count = 0, i, j;
    int blockRowLowBound, blockRowHighBound, blockColLowBound, blockColHighBound;
    blockRowLowBound = rowLowBound(m, row);
    blockRowHighBound = rowHighBound(m, row);
    blockColLowBound = colLowBound(n, col);
    blockColHighBound = colHighBound(n, col);
    for(i = blockRowLowBound ; i < blockRowHighBound ; i++){
        for(j = blockColLowBound ; j < blockColHighBound ; j++) {
            if(indices[i][j][val] == -1){
                continue;
            }
            count ++;
        }
    }
    return count;
}



/*
 * This function builds an array of a constraint of a block by a given length.
 */
int* getConstraintBlock(int val, int row, int col, struct sudokuManager *manager, int ***indices, int constraintLen){
    int *constraint;
    int m = manager->m, n = manager->n;
    int count = 0, i, j;
    int blockRowLowBound = rowLowBound(m, row);
    int blockRowHighBound = rowHighBound(m, row);
    int blockColLowBound = colLowBound(n, col);
    int blockColHighBound = colHighBound(n, col);

    constraint = malloc(constraintLen* sizeof(int));
    if(constraint == NULL) {
        printAllocFailed();
        return NULL;
    }
    for(i = blockRowLowBound ; i < blockRowHighBound ; i++){
        for(j = blockColLowBound ; j < blockColHighBound ; j++) {
            if(indices[i][j][val] == -1){
                continue;
            }
            constraint[count] = indices[i][j][val];
            count++;
        }
    }
    return constraint;
}

/*
 * This function builds an array of a constraint of a col by a given length.
 */
int getConstraintCellLength(int row, int col, struct sudokuManager *manager, int ***indices){
    int count = 0, val;
    for(val = 0; val < boardLen(manager) ; val++){
        if(indices[row][col][val] == -1){
            continue;
        }
        count++;
    }
    return count;
}


/*
 * This function builds an array of a constraint of a cell by a given length.
 */
int* getConstraintCell(int row, int col, struct sudokuManager *manager, int ***indices, int constraintLen){
    int count = 0, val;
    int *constraint;
    constraint = malloc(constraintLen* sizeof(int));
    if(constraint == NULL) {
        printAllocFailed();
        return NULL;
    }
    for(val = 0; val < boardLen(manager); val++){
        if(indices[row][col][val] == -1){
            continue;
        }
        constraint[count] = indices[row][col][val];
        count++;
    }
    return constraint;
}

/*
 * This function creates an array and fills it with ones
 */
double* onesArray(int len){
    int i;
    double* array = (double*)malloc(len* sizeof(double));
    if(array == NULL){
        printAllocFailed();
        return NULL;
    }
    for(i = 0 ; i < len ; i ++){
        array[i] = 1.0;
    }
    return array;
}

int solveGurobi(struct sudokuManager *manager, GurobiOption type, int* retBoard){
    int i, j, k, b, index;
    int constraintLength;
    int N = boardLen(manager);
    GRBenv    *env   = NULL;
    GRBmodel  *model = NULL;
    int       error  = 0;
    double    *sol; /* V */
    int       *ind; /* constraint */ /* V */
    double    *val; /* V */
    double    *obj; /* V */
    char      *vtype; /* V */
    int       optimstatus;
    double    objval; /* */
    int       amountOfVariables;
    int       staticInd[1];
    double    staticVal[1];
    int ***indices = init3DArray(N);
    if(indices == NULL){
        return -2; /* terminate program */
    }

    amountOfVariables = update3DIndices(manager, indices);

    /* Create environment - log file is mip1.log */
    error = GRBloadenv(&env, "mip1.log");
    if (error) {
        printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
        free3DArray(indices, N);
        return -1; /* gurobi error - do not terminate program */
    }

    error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if (error) {
        printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
        free3DArray(indices, N);
        GRBfreeenv(env);
        return -1;
    }

    /* Create an empty model named "mip1" */
    error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
    if (error) {
        printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
        free3DArray(indices, N);
        GRBfreeenv(env);
        return -1;
    }

    /* Add variables */
    srand (time(NULL));
    /* randomizes coefficients for objective function */

    obj = malloc(amountOfVariables*sizeof(double));
    if(obj == NULL){
        printAllocFailed();
        free3DArray(indices, N);
        GRBfreemodel(model);
        GRBfreeenv(env);
        return -2;
    }
    for (i = 0; i < amountOfVariables; i++) {
        obj[i] = randRange(1.0, (double)N);
    }

    /* variable types - for objective function */
    /* other options: GRB_INTEGER, GRB_CONTINUOUS */

    vtype = malloc(amountOfVariables*sizeof(char));
    if(vtype == NULL){
        printAllocFailed();
        free(obj);
        free3DArray(indices, N);
        GRBfreemodel(model);
        GRBfreeenv(env);
        return -2;
    }

    initVariableType(type, vtype, amountOfVariables);

    /* add variables to model */
    error = GRBaddvars(model, amountOfVariables, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
    if (error) {
        printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
        free3DArray(indices, N);
        GRBfreemodel(model);
        GRBfreeenv(env);
        free(obj);
        free(vtype);
        return -1;
    }

    /* Change objective sense to maximization */
    error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
    if (error) {
        printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
        free3DArray(indices, N);
        free(obj);
        free(vtype);
        GRBfreemodel(model);
        GRBfreeenv(env);
        return -1;
    }

    /* update the model - to integrate new variables */

    error = GRBupdatemodel(model);
    if (error) {
        printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
        free3DArray(indices, N);
        free(obj);
        free(vtype);
        GRBfreemodel(model);
        GRBfreeenv(env);
        return -1;
    }


    /* First constraint: x + 2 y + 3 z <= 5 */
    /* ROWS */
    for(i = 0; i < N ; i++){
        for(k = 1; k <= N ; k++){
            constraintLength = getConstaintRowLength(k, i, manager, indices);
            if(constraintLength == 0){
                continue;
            }
            ind = getConstraintRow(k, i, manager, indices, constraintLength);
            if(ind == NULL){
                free3DArray(indices, N);
                free(obj);
                free(vtype);
                GRBfreemodel(model);
                GRBfreeenv(env);
                return -2;
            }
            val = onesArray(constraintLength);
            if (val == NULL){
                free(ind);
                free3DArray(indices, N);
                free(obj);
                free(vtype);
                GRBfreemodel(model);
                GRBfreeenv(env);
                return -2;
            }
            error = GRBaddconstr(model, constraintLength, ind, val, GRB_EQUAL, 1, NULL);
            free(ind);
            free(val);
            if (error) {
                printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                free3DArray(indices, N);
                free(obj);
                free(vtype);
                GRBfreemodel(model);
                GRBfreeenv(env);
                return -1;
            }

        }
    }

    /* COLS */
    for(i = 0; i < N ; i++){
        for(k = 1; k <= N ; k++){
            constraintLength = getConstaintColLength(k, j, manager, indices);
            if(constraintLength == 0){
                continue;
            }
            ind = getConstraintCol(k, j, manager, indices, constraintLength);
            if(ind == NULL){
                free3DArray(indices, N);
                free(obj);
                free(vtype);
                GRBfreemodel(model);
                GRBfreeenv(env);
                return -2;
            }
            val = onesArray(constraintLength);
            if (val == NULL){
                free(ind);
                free3DArray(indices, N);
                free(obj);
                free(vtype);
                GRBfreemodel(model);
                GRBfreeenv(env);
                return -2;
            }
            error = GRBaddconstr(model, constraintLength, ind, val, GRB_EQUAL, 1, NULL);
            free(ind);
            free(val);
            if (error) {
                printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                free3DArray(indices, N);
                free(obj);
                free(vtype);
                GRBfreemodel(model);
                GRBfreeenv(env);
                return -1;
            }

        }
    }

    /* CELLS */
    for(i = 0; i < N ; i++){
        for(j = 0; j < N ; j++){
            constraintLength = getConstraintCellLength(i, j, manager, indices);
            if(constraintLength == 0){
                continue;
            }
            ind = getConstraintCell(i, j, manager, indices, constraintLength);
            if(ind == NULL){
                free3DArray(indices, N);
                free(obj);
                free(vtype);
                GRBfreemodel(model);
                GRBfreeenv(env);
                return -2;
            }
            /* coefficients (according to variables in "ind") */
            val = onesArray(constraintLength);
            if (val == NULL){
                free(ind);
                free3DArray(indices, N);
                free(obj);
                free(vtype);
                GRBfreemodel(model);
                GRBfreeenv(env);
                return -2;
            }
            /* add constraint to model - note size constraintLength + operator GRB_EQUAL */
            error = GRBaddconstr(model, constraintLength, ind, val, GRB_EQUAL, 1, NULL);
            free(ind);
            free(val);
            if (error) {
                printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                free3DArray(indices, N);
                free(obj);
                free(vtype);
                GRBfreemodel(model);
                GRBfreeenv(env);
                return -1;
            }
        }
    }

    /* BLOCKS - MUST ADD we only need one constraint per block for each value */

    for(b = 0; b < N ; b++){ /* b = #block */
        for(k = 1; k <= N ; k++){ /* k = value */
            getFirstIndexInBlock(manager, b, &i, &j);
            constraintLength = getConstraintBlockLength(k, i, j, manager, indices);
            if(constraintLength == 0){
                continue;
            }
            ind = getConstraintBlock(k, i, j, manager, indices, constraintLength);
            if(ind == NULL){
                free3DArray(indices, N);
                free(obj);
                free(vtype);
                GRBfreemodel(model);
                GRBfreeenv(env);
                return -2;
            }
            /* coefficients (according to variables in "ind") */
            val = onesArray(constraintLength);
            if (val == NULL){
                free(ind);
                free3DArray(indices, N);
                free(obj);
                free(vtype);
                GRBfreemodel(model);
                GRBfreeenv(env);
                return -2;
            }
            /* add constraint to model - note size constraintLength + operator GRB_EQUAL */
            error = GRBaddconstr(model, constraintLength, ind, val, GRB_EQUAL, 1, NULL);
            free(ind);
            free(val);
            if (error) {
                printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                free3DArray(indices, N);
                free(obj);
                free(vtype);
                GRBfreemodel(model);
                GRBfreeenv(env);
                return -1;
            }
        }
    }
    /* MAKE SURE EVERY VARIABLE >=0 */
    if(type == CONTINUOUS){
        for(i = 0; i < N; i ++){
            for(j = 0; j < N; j ++){
                for(k = 0; k < N; k ++ ){
                    if(indices[i][j][k]==-1){
                        continue;
                    }
                    staticInd[0] = indices[i][j][k];
                    staticVal[0] = 1;
                    error = GRBaddconstr(model, 1, ind, val, GRB_GREATER_EQUAL, 0, NULL);
                    if (error) {
                        printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                        free3DArray(indices, N);
                        free(obj);
                        free(vtype);
                        GRBfreemodel(model);
                        GRBfreeenv(env);
                        return -1;
                    }
                }
            }
        }
    }



    /* Optimize model - need to call this before calculation */
    error = GRBoptimize(model);
    if (error) {
        printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
        free3DArray(indices, N);
        free(obj);
        free(vtype);
        GRBfreemodel(model);
        GRBfreeenv(env);
        return -1;
    }

    /* Write model to 'mip1.lp' - this is not necessary but very helpful */
    error = GRBwrite(model, "mip1.lp");
    if (error) {
        printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
        free3DArray(indices, N);
        free(obj);
        free(vtype);
        GRBfreemodel(model);
        GRBfreeenv(env);
        return -1;
    }

    /* Get solution information */

    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if (error) {
        printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
        free3DArray(indices, N);
        free(obj);
        free(vtype);
        GRBfreemodel(model);
        GRBfreeenv(env);
        return -1;
    }

    /* get the objective -- the optimal result of the function */
    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
    if (error) {
        printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
        free3DArray(indices, N);
        free(obj);
        free(vtype);
        GRBfreemodel(model);
        GRBfreeenv(env);
        return -1;
    }

    /* get the solution - the assignment to each variable */

    sol = (double*)malloc(amountOfVariables* sizeof(double));
    if(sol == NULL){
        printAllocFailed();
        free3DArray(indices, N);
        free(obj);
        free(vtype);
        GRBfreemodel(model);
        GRBfreeenv(env);
        return -2;
    }

    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, amountOfVariables, sol);
    if (error) {
        printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
        free3DArray(indices, N);
        free(obj);
        free(vtype);
        free(sol)
        GRBfreemodel(model);
        GRBfreeenv(env);
        return -1;
    }

    /* print results */
    printf("\nOptimization complete\n");

    /* solution found */
    if (optimstatus == GRB_OPTIMAL) {
        printf("Optimal objective: %.4e\n", objval);
        for(i = 0; i < amountOfVariables ; i++){
            printf("variable i=%d is equal to %.2f, \n", i, sol[i]);
        }

        if(type != CONTINUOUS){
            for(i = 0; i < N; i++){
                for(j = 0; j < N; j++){
                    index = matIndex(manager->m, manager->n, i, j);
                    if(manager->board[index]!= 0){
                        retBoard[index] = manager->board[index];
                    }
                    else{
                        for (k = 0; k < N ; k++) {
                            if(sol[indices[i][j][k]] == 1){
                                retBoard[index] = k + 1;
                                break;
                            }
                        }
                    }
                }
            }
        }

    }
        /* no solution found */
    else if (optimstatus == GRB_INF_OR_UNBD) {
        printf("Model is infeasible or unbounded\n");
    }
        /* error or calculation stopped */
    else {
        printf("Optimization was stopped early\n");
    }

    /* IMPORTANT !!! - Free model and environment */
    GRBfreemodel(model);
    GRBfreeenv(env);

    return 0;
}

