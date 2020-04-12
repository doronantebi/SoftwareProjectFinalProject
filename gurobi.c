#include "gurobi.h"


#include <stdlib.h>
#include <stdio.h>
#include "gurobi_sys"
#include "utilitiesBoardManager.h"
#include "main_aux.h"
#include <time.h>








/*
 * This method returns index for (i, j, k) in a three dimensional array,
 * of which all dimensions are in size len.
 */
int threeDIndex(int len, int i, int j, int k){
    return (i * (len * len)) + (j * len) + k;
}


/*
 * This function gets a sudokuManager, and 3 dimensional array initialized with zeros.
 * will update the ***indices such that a cell will contain -1 if we don't want it to become a variable,
 * and with its variable index.
 * the function returns the amount of variables we need in our gurobi program.
 */
int update3DIndices(struct sudokuManager *manager, int *indices){
    int length = boardLen(manager);
    int* board = manager->board;
    int row, col, m = manager->m, n = manager->n, val = 0;
    int i, j, height;
    int blockRowLowBound, blockRowHighBound, blockColLowBound, blockColHighBound;
    int count = 0;
    for (row = 0; row < length; row++) {
        for(col = 0; col < length; col++){
            val = board[matIndex(m,n,row,col)];
            if(val == 0){ /* cell is free */
                continue;
            }
            /* update for all values for this cell */
            for(height = 0; height < length; height++){
                indices[threeDIndex(length, row, col, height)] = -1; /* */
            }
            /* update for all row */
            for(j = 0; j < length; j++){
                indices[threeDIndex(length, row, j, val)] = -1;
            }
            /* update for all column */
            for(i = 0; i < length; i++){
                indices[threeDIndex(length, i, col, val)] = -1;
            }
            /* update for all block */
            blockRowLowBound = rowLowBound(m, row);
            blockRowHighBound = rowHighBound(m, row);
            blockColLowBound = colLowBound(n, col);
            blockColHighBound = colHighBound(n, col);
            for(i = blockRowLowBound ; i < blockRowHighBound ; i++){
                for(j = blockColLowBound ; j < blockColHighBound ; j++) {
                    indices[threeDIndex(length, i, j, val)] = -1;
                }
            }
        }
    }
    for(row = 0; row < length; row++){
        for(col = 0; col<length; col++){
            for (height = 0; height < length ;height ++) {
                if(indices[threeDIndex(length, row, col, height)] != -1){ /* only if we want to have this variable */
                    indices[threeDIndex(length, row, col, height)] = count; /* update in the relevant cell its variable index number  */
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
int *init3DArray(int length){
    int *array = (int *)calloc(length * length * length, sizeof(int));
    if(array == NULL){
        printAllocFailed();
        return NULL;
    }
    return array;
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
int getConstraintRowLength(int val, int row, struct sudokuManager *manager, int *indices){
    int length = boardLen(manager);
    int col, count = 0;
    for(col = 0; col < length; col++){
        if(indices[threeDIndex(length, row, col, val)] == -1){
            continue;
        }
        count ++;
    }
    return count;
}

/*
 * This function builds an array of a constraint of a row by a given length.
 */
int *getConstraintRow(int val, int row, struct sudokuManager *manager, int *indices, int constraintLen){
    int length = boardLen(manager);
    int col, count = 0;
    int *constraint;
    constraint = malloc(constraintLen* sizeof(int));
    if(constraint == NULL) {
        printAllocFailed();
        return NULL;
    }
    for(col = 0; col < length; col++){
        if(indices[threeDIndex(length, row, col, val)] == -1){
            continue;
        }
        constraint[count] = indices[threeDIndex(length, row, col, val)];
        count++;
    }
    return constraint;
}

/*
 * This method returns the size of the constraint built on a col
 */
int getConstraintColLength(int val, int col, struct sudokuManager *manager, int *indices){
    int length = boardLen(manager);
    int row, count = 0;
    for(row = 0; row < length; row++){
        if(indices[threeDIndex(length, row, col, val)] == -1){
            continue;
        }
        count ++;
    }
    return count;
}


/*
 * This function builds an array of a constraint of a col by a given length.
 */
int *getConstraintCol(int val, int col, struct sudokuManager *manager, int *indices, int constraintLen){
    int length = boardLen(manager);
    int row, count = 0;
    int *constraint;
    constraint = malloc(constraintLen* sizeof(int));
    if(constraint == NULL) {
        printAllocFailed();
        return NULL;
    }
    for(row = 0; row < length; row++){
        if(indices[threeDIndex(length, row, col, val)] == -1){
            continue;
        }
        constraint[count] = indices[threeDIndex(length, row, col, val)];
        count++;
    }
    return constraint;
}


/*
 * This method returns the size of the constraint built on a block
 */
int getConstraintBlockLength(int val, int row, int col, struct sudokuManager *manager, int *indices){
    int m = manager->m, n = manager->n;
    int count = 0, i, j;
    int blockRowLowBound, blockRowHighBound, blockColLowBound, blockColHighBound;
    blockRowLowBound = rowLowBound(m, row);
    blockRowHighBound = rowHighBound(m, row);
    blockColLowBound = colLowBound(n, col);
    blockColHighBound = colHighBound(n, col);
    for(i = blockRowLowBound ; i < blockRowHighBound ; i++){
        for(j = blockColLowBound ; j < blockColHighBound ; j++) {
            if(indices[threeDIndex(boardLen(manager), i, j, val)] == -1){
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
int* getConstraintBlock(int val, int row, int col, struct sudokuManager *manager, int *indices, int constraintLen){
    int *constraint;
    int m = manager->m, n = manager->n;
    int count = 0, i, j, length = boardLen(manager);
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
            if(indices[threeDIndex(length, i, j, val)] == -1){
                continue;
            }
            constraint[count] = indices[threeDIndex(length, i, j, val)];
            count++;
        }
    }
    return constraint;
}

/*
 * This function builds an array of a constraint of a col by a given length.
 */
int getConstraintCellLength(int row, int col, struct sudokuManager *manager, int *indices){
    int count = 0, val, length = boardLen(manager);
    for(val = 0; val < boardLen(manager) ; val++){
        if(indices[threeDIndex(length, row, col, val)] == -1){
            continue;
        }
        count++;
    }
    return count;
}


/*
 * This function builds an array of a constraint of a cell by a given length.
 */
int* getConstraintCell(int row, int col, struct sudokuManager *manager, int *indices, int constraintLen){
    int count = 0, val, length = boardLen(manager);
    int *constraint;
    constraint = malloc(constraintLen* sizeof(int));
    if(constraint == NULL) {
        printAllocFailed();
        return NULL;
    }
    for(val = 0; val < boardLen(manager); val++){
        if(indices[threeDIndex(length, row, col, val)] == -1){
            continue;
        }
        constraint[count] = indices[threeDIndex(length, row, col, val)];
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

/*
 * This method frees gurobi model and environment, and everything related.
 */
void freeGurobi(double *obj, char *vtype, GRBenv *env, GRBmodel *model) {
    free(obj);
    free(vtype);
    if (model != NULL){
        GRBfreemodel(model);
    }
    if (env != NULL){
        GRBfreeenv(env);
    }
}

/*
 * This method set constraints for each row of the sudoku board.
 * It returns -2 if memory allocation failed, -1 if gurobi had an error and 0 otherwise.
 */
int setRowConstraints(struct sudokuManager *manager, int N, int *indices, GRBmodel *model){
    int i, k, constraintLength, error, *ind;
    double *val;

    for(i = 0; i < N ; i++){
        for(k = 1; k <= N ; k++){
            constraintLength = getConstraintRowLength(k, i, manager, indices);
            if(constraintLength == 0){
                continue;
            }
            ind = getConstraintRow(k, i, manager, indices, constraintLength);
            if(ind == NULL){
                return -2;
            }
            val = onesArray(constraintLength);
            if (val == NULL){
                free(ind);
                return -2;
            }
            error = GRBaddconstr(model, constraintLength, ind, val, GRB_EQUAL, 1, NULL);
            free(ind);
            free(val);
            if (error) {
                printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                return -1;
            }

        }
    }
    return 0;
}

/*
 * This method set constraints for each column of the sudoku board.
 * It returns -2 if memory allocation failed, -1 if gurobi had an error and 0 otherwise.
 */
int setColConstraints(struct sudokuManager *manager, int N, int *indices, GRBmodel *model){
    int j, k, constraintLength, error, *ind;
    double *val;

    for(j = 0; j < N ; j++){
        for(k = 1; k <= N ; k++){
            constraintLength = getConstraintColLength(k, j, manager, indices);
            if(constraintLength == 0){
                continue;
            }
            ind = getConstraintCol(k, j, manager, indices, constraintLength);
            if(ind == NULL){

                return -2;
            }
            val = onesArray(constraintLength);
            if (val == NULL){
                free(ind);
                return -2;
            }
            error = GRBaddconstr(model, constraintLength, ind, val, GRB_EQUAL, 1, NULL);
            free(ind);
            free(val);
            if (error) {
                printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                return -1;
            }
        }
    }
    return 0;
}

/*
 * This method set constraints for each cell of the sudoku board.
 * It returns -2 if memory allocation failed, -1 if gurobi had an error and 0 otherwise.
 */
int setCellsConstraints(struct sudokuManager *manager, int N, int *indices, GRBmodel *model){
    int i, j, constraintLength, error, *ind;
    double *val;

    for(i = 0; i < N ; i++){
        for(j = 0; j < N ; j++){
            constraintLength = getConstraintCellLength(i, j, manager, indices);
            if(constraintLength == 0){
                continue;
            }
            ind = getConstraintCell(i, j, manager, indices, constraintLength);
            if(ind == NULL){
                return -2;
            }
            /* coefficients (according to variables in "ind") */
            val = onesArray(constraintLength);
            if (val == NULL){
                free(ind);
                return -2;
            }
            /* add constraint to model - note size constraintLength + operator GRB_EQUAL */
            error = GRBaddconstr(model, constraintLength, ind, val, GRB_EQUAL, 1, NULL);
            free(ind);
            free(val);
            if (error) {
                printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                return -1;
            }
        }
    }
    return 0;
}

/*
 * This method set constraints for each block of the sudoku board.
 * It returns -2 if memory allocation failed, -1 if gurobi had an error and 0 otherwise.
 */
int setBlocksConstraints(struct sudokuManager *manager, int N, int *indices, GRBmodel *model){
    int i, j, k , b, constraintLength, error, *ind;
    double *val;

    for(b = 0; b < N ; b++){ /* b = #block */
        for(k = 1; k <= N ; k++){ /* k = value */
            getFirstIndexInBlock(manager->m, manager->n, b, &i, &j);
            constraintLength = getConstraintBlockLength(k, i, j, manager, indices);
            if(constraintLength == 0){
                continue;
            }
            ind = getConstraintBlock(k, i, j, manager, indices, constraintLength);
            if(ind == NULL){
                return -2;
            }
            /* coefficients (according to variables in "ind") */
            val = onesArray(constraintLength);
            if (val == NULL){
                free(ind);
                return -2;
            }
            /* add constraint to model - note size constraintLength + operator GRB_EQUAL */
            error = GRBaddconstr(model, constraintLength, ind, val, GRB_EQUAL, 1, NULL);
            free(ind);
            free(val);
            if (error) {
                printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                return -1;
            }
        }
    }
    return 0;
}

/*
 * This method sets a constraint for each variable to be non-negative.
 * It returns -1 if gurobi had an error. Otherwise, it returns 0.
 */
int setNonnegativityConstraints(struct sudokuManager *manager, int N, int *indices, GRBmodel *model){
    int i, j, k, error;
    int staticInd[1];
    double staticVal[1];

    for(i = 0; i < N; i ++){
        for(j = 0; j < N; j ++){
            for(k = 0; k < N; k ++ ){
                if(indices[threeDIndex(N, i, j, k)] == -1){
                    continue;
                }
                staticInd[0] = indices[threeDIndex(N, i, j, k)];
                staticVal[0] = 1;
                error = GRBaddconstr(model, 1, ind, val, GRB_GREATER_EQUAL, 0, NULL);
                if (error) {
                    printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                    return -1;
                }
            }
        }
    }
    return 0;
}

/*
 * This method allocates all memory needed for Gurobi's operation.
 * If Gurobi's environment or model allocation fails, it returns -1.
 * If other allocations fail, it returns -2.
 * Otherwise, it returns 0.
 */
int initGurobi(GRBenv **pEnv, GRBmodel **pModel,
                double **pObj, char **pVtype, int amountOfVariables){
    int error = 0;

    /* Create environment - log file is mip1.log */
    error = GRBloadenv(pEnv, "mip1.log");
    if (error) {
        printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(*pEnv));
        freeGurobi(*pObj, *pVtype, *pEnv, *pModel);
        return -1; /* gurobi error - do not terminate program */
    }

    /* Create an empty model named "mip1" */
    error = GRBnewmodel(*pEnv, pModel, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
    if (error) {
        printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(*pEnv));
        freeGurobi(*pObj, *pVtype, *pEnv, *pModel);
        return -1;
    }

    *pObj = malloc(amountOfVariables * sizeof(double));
    if(*pObj == NULL){
        printAllocFailed();
        freeGurobi(*pObj, *pVtype, *pEnv, *pModel);
        return -2;
    }

    *pVtype = malloc(amountOfVariables * sizeof(char));
    if(*pVtype == NULL){
        printAllocFailed();
        freeGurobi(*pObj, *pVtype, *pEnv, *pModel);
        return -2;
    }

    return 0;
}

/*
 * This method solves manager using ILP or LP depending on type.
 * type == BINARY ---> *retBoard is the solution for the board if exists,
 * and if there is no solution, *retBoard == NULL.
 * type == CONTINUOUS ---> user must use with *retBoard == NULL.
 * The method returns -1 if Gurobi had an error, -2 if memory allocation failed, and 0 otherwise.
 */
int solveGurobi(struct sudokuManager *manager, GurobiOption type, int **retBoard,
                double *sol, int *indices, int amountOfVariables){
    int i, j, k, index, res;
    int N = boardLen(manager);
    GRBenv    *env   = NULL;
    GRBmodel  *model = NULL;
    int       error  = 0;
    double    *obj = NULL; /* V */
    char      *vtype = NULL; /* V */
    int       optimstatus;
    double    objval; /* */
    /*
    double    *sol = NULL;
    int       amountOfVariables = 0;
    int       *indices = NULL;
    */

    res = initGurobi(manager, &env, &model, &obj, &vtype, amountOfVariables);
    if (res){
        return res;
    }

    /* everything is allocated */

    error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if (error) {
        printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
        freeGurobi(obj, vtype, env, model);
        return -1;
    }


    /* Add variables */
    srand (time(NULL));
    /* randomizes coefficients for objective function */

    for (i = 0; i < amountOfVariables; i++) {
        obj[i] = randRangeDouble(1.0, (double)N);
    }


    /* variable types - for objective function */
    /* other options: GRB_INTEGER, GRB_CONTINUOUS */
    initVariableType(type, vtype, amountOfVariables);

    /* add variables to model */
    error = GRBaddvars(model, amountOfVariables, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
    if (error) {
        printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
        freeGurobi(obj, vtype, env, model);
        return -1;
    }

    /* Change objective sense to maximization */
    error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
    if (error) {
        printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
        freeGurobi(obj, vtype, env, model);
        return -1;
    }

    /* update the model - to integrate new variables */

    error = GRBupdatemodel(model);
    if (error) {
        printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
        freeGurobi(obj, vtype, env, model);
        return -1;
    }


    /* ROWS */
    res = setRowConstraints(manager, N, indices, model);
    if (res){
        freeGurobi(obj, vtype, env, model);
        return res;
    }

    /* COLS */
    res = setColConstraints(manager, N, indices, model);
    if (res){
        freeGurobi(obj, vtype, env, model);
        return res;
    }

    /* CELLS */
    res = setCellsConstraints(manager, N, indices, model);
    if (res){
        freeGurobi(obj, vtype, env, model);
        return res;
    }

    /* BLOCKS - MUST ADD we only need one constraint per block for each value */
    res = setBlocksConstraints(manager, N, indices, model);
    if (res){
        freeGurobi(obj, vtype, env, model);
        return res;
    }

    /* MAKE SURE EVERY VARIABLE >=0 */
    if(type == CONTINUOUS){
        res = setNonnegativityConstraints(manager, N, indices, model);
        if (res){
            freeGurobi(obj, vtype, env, model);
            return res;
        }
    }


    /* Optimize model - need to call this before calculation */
    error = GRBoptimize(model);
    if (error) {
        printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
        freeGurobi(obj, vtype, env, model);
        return -1;
    }

    /* Write model to 'mip1.lp' - this is not necessary but very helpful */
    error = GRBwrite(model, "mip1.lp");
    if (error) {
        printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
        freeGurobi(obj, vtype, env, model);
        return -1;
    }

    /* Get solution information */

    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if (error) {
        printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
        freeGurobi(obj, vtype, env, model);
        return -1;
    }

    /* get the objective -- the optimal result of the function */
    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
    if (error) {
        printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
        freeGurobi(obj, vtype, env, model);
        return -1;
    }

    /* get the solution - the assignment to each variable */
    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, amountOfVariables, sol);
    if (error) {
        printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
        freeGurobi(obj, vtype, env, model);
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

        if (type != CONTINUOUS){
            for(i = 0; i < N; i++){
                for(j = 0; j < N; j++){
                    index = matIndex(manager->m, manager->n, i, j);
                    if(manager->board[index] != 0){
                        (*retBoard)[index] = manager->board[index];
                    }
                    else{
                        for (k = 0; k < N ; k++) {
                            if(sol[indices[threeDIndex(N, i, j, k)]] == 1){
                                (*retBoard)[index] = k + 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
        else{
            *retBoard = NULL; /* MAKE SURE!!! I am not sure*/
        }

    }
        /* no solution found */
    else if (optimstatus == GRB_INF_OR_UNBD) {
        printf("Model is infeasible or unbounded\n");
        free(*retBoard);
        *retBoard = NULL; /* */
    }
        /* error or calculation stopped */
    else {
        printf("Optimization was stopped early\n");
    }

    /* IMPORTANT !!! - Free model and environment */
    freeGurobi(obj, vtype, env, model);

    return 0;
}


/*
 * This method solves the current board using ILP.
 * The solution is returned through retBoard.
 * The method returns -1 if Gurobi had an error, -2 if memory allocation failed, and 0 otherwise.
 */
int solveBoard(struct sudokuManager *manager, int **retBoard){
    int N = boardLen(manager), amountOfVariables, res;
    int *indices = NULL;
    double *sol = NULL;
    indices = init3DArray(N);
    if (indices == NULL){
        return -2; /* terminate program */
    }

    amountOfVariables = update3DIndices(manager, indices);

    sol = (double*)malloc(amountOfVariables * sizeof(double));
    if(sol == NULL){
        printAllocFailed();
        free(indices);
        return -2;
    }

    res = solveGurobi(manager, BINARY, retBoard, sol, indices, amountOfVariables);

    free(indices);
    free(sol);
    return res;
}




/* This method solves the current board using LP.
* The method returns -1 if Gurobi had an error, -2 if memory allocation failed, and 0 otherwise.
 * It returns all the possible values of cell (row, col) through *pCellValues, and its length through *pLength.
*/
int guessCellValues(struct sudokuManager *manager, int row, int col,
                    int **pCellValues, int *pLength){
    int N = boardLen(manager), amountOfVariables, res, k, index;
    int *indices = NULL, *retBoard = NULL;
    double *sol = NULL;
    int count = 0;

    indices = init3DArray(N);
    if (indices == NULL){
        return -2; /* terminate program */
    }

    amountOfVariables = update3DIndices(manager, indices);

    sol = (double*)malloc(amountOfVariables * sizeof(double));
    if(sol == NULL){
        printAllocFailed();
        free(indices);
        return -2;
    }

    res = solveGurobi(manager, CONTINUOUS, &retBoard, sol, indices, amountOfVariables);

    if (res){
        free(indices);
        free(sol);
        return res;
    }

    *pCellValues = (int *)malloc(N * sizeof(int));

    if (*pCellValues == NULL){
        free(indices);
        free(sol);
        return -2;
    }

    for (k = 0; k < N; k++){
        index = threeDIndex(N, row, col, k);
        if (indices[index] == -1){
            continue;
        }
        if (sol[indices[index]] != 0){
            (*pCellValues)[count] = k + 1;
            count++;
        }
    }
    *pLength = count;
    *pCellValues = (int *)realloc(*pCellValues, count);
    free(indices);
    free(sol);

    if (*pCellValues == NULL){ /* shouldn't happen */
        return -2;
    }

    return 0;
}

/*
 * This method fills availableValues with the legal values for cell (row, col) of which the score is above
 * threshold, and fills their score in the array scores accordingly.
 * It returns its length in *pLength and scores' sum of the available values (the sum of scores array)
 * in *pSumScores.
 */
void createAvailableValues(struct sudokuManager *manager, int *availableValues, float *scores, float threshold,
                            int *indices, double *sol, int *pLength, float *pSumScores, int row, int col){
    int k, index, N = boardLen(manager);
    *pLength = 0;
    *pSumScores = 0;
    for (k = 0; k < N; k++){
        index = threeDIndex(N, row, col, k);
        if (indices[index] == -1){ /*this value was erroneous from the first place*/
            continue;
        }
        if (sol[indices[index]] >= threshold){ /*score is above the threshold we got*/
            if (!neighbourContainsOnce(manager->board, manager->m, manager->n, row, col, k + 1)){
                /*value is not erroneous for this cell*/
                availableValues[*pLength] = k + 1;
                scores[*pLength] = (float)(sol[indices[index]]);
                (*pSumScores) += scores[*pLength];
                (*pLength)++;
            }
        }
    }
}

/* This method solves the current board using LP.
* The method returns -1 if Gurobi had an error, -2 if memory allocation failed, and 0 otherwise.
 * It returns all the possible values of cell (row, col) through *pCellValues, and its length through *pLength.
*/
int guessSolution(struct sudokuManager *manager,
                  float threshold) {
    int N = boardLen(manager), amountOfVariables, res;
    int *indices = NULL, *retBoard = NULL;
    double *sol = NULL;
    int i, j, k, length;
    int *availableValues = NULL;
    float *scores = NULL, randScore, sumScores, currScore; /*scores is an array of matching scores to availableValues*/

    indices = init3DArray(N);
    if (indices == NULL) {
        return -2; /* terminate program */
    }

    amountOfVariables = update3DIndices(manager, indices);

    sol = (double *) malloc(amountOfVariables * sizeof(double));
    if (sol == NULL) {
        printAllocFailed();
        free(indices);
        return -2;
    }

    res = solveGurobi(manager, CONTINUOUS, &retBoard, sol, indices, amountOfVariables); /*running gurobi*/

    if (res){
        free(indices);
        free(sol);
        return res;
    }

    availableValues = (int *)calloc(N, sizeof(int));
    scores = (float *)calloc(N, sizeof(float));

    if (availableValues == NULL || scores == NULL){
        free(indices);
        free(sol);
        free(availableValues);
        free(scores);
        return -2;
    }

    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            if (isFixedCell(manager, i, j) || manager->board[matIndex(manager->m, manager->n, i, j)] != 0){
                /*if the cell is not empty, we need to continue to the next cell*/
                continue;
            }
            /*length is the actual length of availableValues and scores */
            /*sumScores is the sum of scores of available values for this cell = (i, j) */
            createAvailableValues(manager, availableValues, scores, threshold, indices, sol, &length, &sumScores, i, j);
            randScore = (((float)(rand())) / RAND_MAX) * sumScores; /*getting a random number between 0 and sumScores*/
            currScore = 0;
            for (k = 0; k < length; k++){
                if ((randScore >= currScore) && (randScore <= scores[k] + currScore)){
                    if (doSet(manager, i, j, availableValues[k]) == -1){
                        free(indices);
                        free(sol);
                        free(availableValues);
                        free(scores);
                        return -2;
                    }
                    /*updating the board if the randScore tells us to choose avialableValues[k]*/
                    break;
                }
                else{
                    currScore += scores[k]; /*updating the current score, which is the sum of all scores
                                            * before the current available value*/
                }
            }
        }
    }

    free(indices);
    free(sol);
    free(availableValues);
    free(scores);
    return 0;
}
