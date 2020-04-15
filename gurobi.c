

#include "gurobi.h"
#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c.h"
#include "utilitiesBoardManager.h"
#include <time.h>


/*
 * This function is raffles a double between min and max.
 */
double randRangeDouble(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

/*
 * This method returns index for (i, j, k) in a three dimensional array,
 * of which all dimensions are in size len.
 */
int threeDIndex(int len, int i, int j, int k){
    return (i * (len * len)) + (j * len) + k;
}


/*
 * This function gets a sudokuManager, and 3 dimensional array initialized with zeros.
 * will update indices such that a cell will contain -1 if we don't want it to become a variable,
 * and otherwise with its variable index.
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
            /*printf("cell (%d, %d), value: %d\n", row, col, val);*/
            if(val == 0){ /* cell is free */
                continue;
            }
            val--;
            /*printf("deleting values for current cell\n");*/
            /* update for all values for this cell */
            for(height = 0; height < length; height++){
                /*printf("deleted value %d for cell (%d, %d)\n", height+1, row, col);*/
                if(indices[threeDIndex(length, row, col, height)] == 0) {
                    indices[threeDIndex(length, row, col, height)] = -1;
                }
            }

            /* update for all row */
            /*printf("deleting value %d for all row\n", val);*/
            for(j = 0; j < length; j++){
                /*printf("deleted value %d for cell (%d, %d)\n", val, row, j);*/
                if(indices[threeDIndex(length, row, j, val)] == 0) {
                    indices[threeDIndex(length, row, j, val)] = -1;
                }
            }
            /* update for all column */
            /*printf("deleting value %d for all column\n", val);*/
            for(i = 0; i < length; i++){
                /*printf("deleted value %d for cell (%d, %d)\n", val, i, col);*/
                if(indices[threeDIndex(length, i, col, val)] == 0) {
                    indices[threeDIndex(length, i, col, val)] = -1;
                }
            }
            /* update for all block */
            blockRowLowBound = rowLowBound(m, row);
            blockRowHighBound = rowHighBound(m, row);
            blockColLowBound = colLowBound(n, col);
            blockColHighBound = colHighBound(n, col);
            /*printf("deleting value %d for all block\n", val);*/
            for(i = blockRowLowBound ; i < blockRowHighBound ; i++){
                for(j = blockColLowBound ; j < blockColHighBound ; j++) {
                    /*printf("deleted value %d for cell (%d, %d)\n", val, i, j);*/
                    if(indices[threeDIndex(length, i, j, val)] == 0) {
                        indices[threeDIndex(length, i, j, val)] = -1;
                    }
                }
            }
            /* after filling the row, column, cell and block with -1 we set -2 in the cell that
            * represents the actual value(+1) of the cell in manager->board */
            indices[threeDIndex(length, row, col, val)] = -2;
        }
    }
    /*printf("indices left:\n");*/
    for(row = 0; row < length; row++){
        for(col = 0; col < length; col++){
            for (height = 0; height < length ;height ++) {
                val = indices[threeDIndex(length, row, col, height)];
                if((val != -1)&&(val != -2)){ /* only if we want to have this variable */
                    indices[threeDIndex(length, row, col, height)] = count; /* update in the relevant cell
                                                                            * its variable index number  */
                    /*printf("(row, col, value) = (%d, %d, %d) is variable number %d\n", row, col, height+1, count);*/
                    count ++;
                }
            }
        }
    }
    /*printf("finished. count = %d\n", count);*/
    return count; /* returns the amount of variables */
}

/*
 * Initializes a 3D array with zeros.
 */
int *init3DArray(int length){
    int *array = (int *)calloc(length * length * length, sizeof(int));
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
 * This function returns the size of the constraint builds on a row.
 * if it returns -1, it means that there is already a cell in the row with val",
 * therefore, we don't have to make a constraint for it.
 * if it returns 0, it means that no cell in the row that contains val,
 * but val is illegal for all cells in row;
 * Which means that the board can not be solved.
 */
int getConstraintRowLength(int val, int row, struct sudokuManager *manager, int *indices){
    int length = boardLen(manager);
    int col, count = 0;
    for(col = 0; col < length; col++){
        if(indices[threeDIndex(length, row, col, val)] == -1){
            continue;
        }
        if(indices[threeDIndex(length, row, col, val)] == -2){
            return -1;
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
        return NULL;
    }
    for(col = 0; col < length; col++){
        if(indices[threeDIndex(length, row, col, val)] == -1){
            /* none of them can be -2, because we wouldn't build a constraint in that case */
            continue;
        }
        constraint[count] = indices[threeDIndex(length, row, col, val)];
        count++;
    }
    return constraint;
}

/*
 * This method returns the size of the constraint built on a col
 * if it returns -1, it means that there is already a cell in the column with val,
 * therefore, we don't have to make a constraint for it.
 * if it returns 0, it means that no cell in the column col that contains val,
 * but val is illegal for all cells in the column;
 * Which means that the board can not be solved.
 */
int getConstraintColLength(int val, int col, struct sudokuManager *manager, int *indices){
    int length = boardLen(manager);
    int row, count = 0;
    for(row = 0; row < length; row++){
        if(indices[threeDIndex(length, row, col, val)] == -1){
            continue;
        }
        if(indices[threeDIndex(length, row, col, val)] == -2){
            return -1;
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
        return NULL;
    }
    for(row = 0; row < length; row++){
        if(indices[threeDIndex(length, row, col, val)] == -1){
            /* none of them can be -2, because we wouldn't build a constraint in that case */
            continue;
        }
        constraint[count] = indices[threeDIndex(length, row, col, val)];
        count++;
    }
    return constraint;
}


/*
 * This method returns the size of the constraint built on a block
 * if it returns -1, it means that there is already a cell in the block with val,
 * therefore, we don't have to make a constraint for it.
 * if it returns 0, it means that no cell in the block that contains val,
 * but val is illegal for all cells in the block;
 * Which means that the board can not be solved.
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
            if(indices[threeDIndex(boardLen(manager), i, j, val)] == -2){
                return -1;
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
        return NULL;
    }
    for(i = blockRowLowBound ; i < blockRowHighBound ; i++){
        for(j = blockColLowBound ; j < blockColHighBound ; j++) {
            if(indices[threeDIndex(length, i, j, val)] == -1){
                /* none of them can be -2, because we wouldn't build a constraint in that case */
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
 * if it returns -1, it means that there is already a value in that cell,
 * therefore, we don't have to make a constraint for it.
 * if it returns 0, it means that there is no value setted in that cell,
 * but all values are illegal it;
 * Which means that the board can not be solved.
 */
int getConstraintCellLength(int row, int col, struct sudokuManager *manager, int *indices){
    int count = 0, val, length = boardLen(manager);
    for(val = 0; val < boardLen(manager) ; val++){
        if(indices[threeDIndex(length, row, col, val)] == -1){
            continue;
        }
        if(indices[threeDIndex(length, row, col, val)] == -2){
            return -1;
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
        return NULL;
    }
    for(val = 0; val < boardLen(manager); val++){
        if(indices[threeDIndex(length, row, col, val)] == -1){
            /* none of them can be -2, because we wouldn't build a constraint in that case */
            continue;
        }
        constraint[count] = indices[threeDIndex(length, row, col, val)];
        count++;
    }
    return constraint;
}

/*
 * This function creates an array and fills it with double ones
 */
double* onesArray(int len){
    int i;
    double* array = (double*)malloc(len* sizeof(double));
    if(array == NULL){
        return NULL;
    }
    for(i = 0 ; i < len ; i ++){
        array[i] = 1.0;
    }
    return array;
}

/*
 * This method frees Gurobi model and environment, and everything related.
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
 * It returns -2 if memory allocation failed, -1 if Gurobi had an error,
 * returns 1 for finishing building the constraints successfully, and 0 if the board is invalid.
 */
int setRowConstraints(struct sudokuManager *manager, int N, int *indices, GRBmodel *model, GRBenv *env){
    int i, k, constraintLength, error, *ind;
    double *val;

    for(i = 0; i < N ; i++){
        for(k = 0; k < N ; k++){
            constraintLength = getConstraintRowLength(k, i, manager, indices);
            if(constraintLength == -1){
                /* k is setted somewhere in the row */
                continue;
            }
            if(constraintLength == 0){
                /* k is illegal for all cells in that row, the board is invalid */
                return 0;
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
    return 1;
}

/*
 * This method set constraints for each column of the sudoku board.
 * It returns -2 if memory allocation failed, -1 if Gurobi had an error,
 * returns 1 for finishing building the constraints successfully, and 0 if the board is invalid.
 */
int setColConstraints(struct sudokuManager *manager, int N, int *indices, GRBmodel *model, GRBenv *env){
    int j, k, constraintLength, error, *ind;
    double *val;

    for(j = 0; j < N ; j++){
        for(k = 0; k < N ; k++){
            constraintLength = getConstraintColLength(k, j, manager, indices);
            if(constraintLength == -1){
                /* k is setted somewhere in the col */
                continue;
            }
            if(constraintLength == 0){
                /* k is illegal for all cells in that col, the board is invalid */
                return 0;
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
    return 1;
}

/*
 * This method set constraints for each cell of the sudoku board.
 * It returns -2 if memory allocation failed, -1 if Gurobi had an error,
 * returns 1 for finishing building the constraints successfully, and 0 if the board is invalid.
 */
int setCellsConstraints(struct sudokuManager *manager, int N, int *indices, GRBmodel *model, GRBenv *env){
    int i, j, constraintLength, error, *ind;
    double *val;

    for(i = 0; i < N ; i++){
        for(j = 0; j < N ; j++){
            constraintLength = getConstraintCellLength(i, j, manager, indices);
            if(constraintLength == -1){
                /* there is a value in cell (i,j) already */
                continue;
            }
            if(constraintLength == 0){
                /* all values for cell (i,j) are illegal, the board is invalid */
                return 0;
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
    return 1;
}

/*
 * This method set constraints for each block of the sudoku board.
 * It returns -2 if memory allocation failed, -1 if Gurobi had an error,
 * returns 1 for finishing building the constraints successfully, and 0 if the board is invalid.
 */
int setBlocksConstraints(struct sudokuManager *manager, int N, int *indices, GRBmodel *model, GRBenv *env){
    int i, j, k , b, constraintLength, error, *ind;
    double *val;

    for(b = 0; b < N ; b++){ /* b = #block */
        for(k = 0; k < N ; k++){ /* k = value */
            getFirstIndexInBlock(manager->m, manager->n, b, &i, &j);
            constraintLength = getConstraintBlockLength(k, i, j, manager, indices);
            if(constraintLength == -1){
                /* value k is already in the block */
                continue;
            }
            if(constraintLength == 0){
                /* value k is illegal for all cells in the block, the board is invalid */
                return 0;
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
    return 1;
}

/*
 * This method sets a constraint for each variable to be non-negative.
 * It returns -1 if Gurobi had an error. Otherwise, it returns 0.
 */
int setNonnegativityConstraints(int N, int *indices, GRBmodel *model, GRBenv *env){
    int i, j, k, error;
    int staticInd[1];
    double staticVal[1];

    for(i = 0; i < N; i ++){
        for(j = 0; j < N; j ++){
            for(k = 0; k < N; k ++ ){
                if((indices[threeDIndex(N, i, j, k)] == -1)||(indices[threeDIndex(N, i, j, k)] == -2)){
                    continue;
                }
                staticInd[0] = indices[threeDIndex(N, i, j, k)];
                staticVal[0] = 1;
                error = GRBaddconstr(model, 1, staticInd, staticVal, GRB_GREATER_EQUAL, 0, NULL);
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
        freeGurobi(*pObj, *pVtype, *pEnv, *pModel);
        return -2;
    }

    *pVtype = malloc(amountOfVariables * sizeof(char));
    if(*pVtype == NULL){
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
 * The method returns -1 if Gurobi had an error, -2 if memory allocation failed, 1 if we succeeded and
 * found optimal solution, and 0 otherwise.
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
    int       currIndex;

    res = initGurobi(&env, &model, &obj, &vtype, amountOfVariables);
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
    res = setRowConstraints(manager, N, indices, model, env);
    if (res != 1){ /* if it equals 1, constraints setting went successfully */
        freeGurobi(obj, vtype, env, model);
        return res;
    }

    /* COLS */
    res = setColConstraints(manager, N, indices, model, env);
    if (res != 1){ /* if it equals 1, constraints setting went successfully */
        freeGurobi(obj, vtype, env, model);
        return res;
    }

    /* CELLS */
    res = setCellsConstraints(manager, N, indices, model, env);
    if (res != 1){ /* if it equals 1, constraints setting went successfully */
        freeGurobi(obj, vtype, env, model);
        return res;
    }

    /* BLOCKS - MUST ADD we only need one constraint per block for each value */
    res = setBlocksConstraints(manager, N, indices, model, env);
    if (res != 1){ /* if it equals 1, constraints setting went successfully */
        freeGurobi(obj, vtype, env, model);
        return res;
    }

    /* MAKE SURE EVERY VARIABLE >=0 */
    if(type == CONTINUOUS){
        res = setNonnegativityConstraints(N, indices, model, env);
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



    /* print results */
    printf("\nOptimization complete\n");

    /* solution found */
    if (optimstatus == GRB_OPTIMAL) {
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
        printf("Optimal objective: %.4e\n", objval);
        for(i = 0; i < amountOfVariables ; i++){
            printf("variable i=%d is equal to %.2f, \n", i, sol[i]);
        }

        if (type != CONTINUOUS){
            for(i = 0; i < N; i++){
                for(j = 0; j < N; j++){
                    index = matIndex(manager->m, manager->n, i, j);
                    /*printf("cell (%d, %d)\n", i, j);*/
                    if(manager->board[index] != 0){
                        /*printf("manager->board[%d][%d] != 0\n", i, j);*/
                        (*retBoard)[index] = manager->board[index];
                        /*printf("(*retBoard)[%d][%d] = %d\n", i, j, (*retBoard)[index]);*/
                    }
                    else{
                        /*printf("manager->board[%d][%d] == 0\n", i, j);*/
                        for (k = 0; k < N ; k++) {
                            currIndex = indices[threeDIndex(N, i, j, k)];
                            if((currIndex != -1) && (sol[currIndex] == 1.0)){
                                (*retBoard)[index] = k + 1;
                                /*printf("(*retBoard)[%d][%d] = %d\n", i, j, (*retBoard)[index]);*/
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
        /* no solution found */
    else{
        if (optimstatus == GRB_INF_OR_UNBD) {
            printf("Model is infeasible or unbounded\n");
            freeGurobi(obj, vtype, env, model);
            return 0;

        }
            /* error or calculation stopped */
        else {
            printf("Optimization was stopped early\n");
            freeGurobi(obj, vtype, env, model);
            return -1;
        }
    }


    /* IMPORTANT !!! - Free model and environment */
    freeGurobi(obj, vtype, env, model);

    return 1; /* if we got here, we didn't fail and there is optimal solution */
}


/*
 * This method solves the current board using ILP.
 * The solution is returned through retBoard.
 * The method returns -1 if Gurobi had an error,
 * -2 if memory allocation failed
 * 1 if *retBoard is filled with a solution (the board is solvable),
 * and 0 if the board is unsolvable.
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
        free(indices);
        return -2;
    }

    res = solveGurobi(manager, BINARY, retBoard, sol, indices, amountOfVariables);

    free(indices);
    free(sol);

    return res;
}




/* This method solves the current board using LP.
* The method returns -1 if Gurobi had an error,
 * -2 if memory allocation failed,
 * 1 if we succeeded in guessing the values (the board is solvable),
 * and 0 if the board is unsolvable.
 * It returns all the possible values of cell (row, col) through *pCellValues its score through *pScores.
 * The length of *pCellValues and *pScores is returned through *pLength.
 * User needs to free *pCellValues and *pScores iff return value == 1.
*/
int guessCellValues(struct sudokuManager *manager, int row, int col,
                    int **pCellValues, double **pScores, int *pLength){
    int N = boardLen(manager), amountOfVariables, res, k, index;
    int *indices = NULL, *retBoard = NULL, *realloc1;
    double *sol = NULL, *realloc2;
    int count = 0;

    indices = init3DArray(N);
    if (indices == NULL){
        return -2; /* terminate program */
    }

    amountOfVariables = update3DIndices(manager, indices);

    sol = (double*)malloc(amountOfVariables * sizeof(double));
    if(sol == NULL){
        free(indices);
        return -2;
    }

    res = solveGurobi(manager, CONTINUOUS, &retBoard, sol, indices, amountOfVariables);

    if (res == -1 || res == -2 || res == 0){
        free(indices);
        free(sol);
        return res;
    }

    *pScores = (double *)malloc(N * sizeof(double));
    *pCellValues = (int *)malloc(N * sizeof(int));

    if (*pCellValues == NULL || *pScores == NULL){
        free(*pScores);
        free(*pCellValues);
        free(indices);
        free(sol);
        return -2;
    }

    for (k = 0; k < N; k++){
        index = threeDIndex(N, row, col, k);
        if (indices[index] == -1){
            continue;
        }
        if (sol[indices[index]] > 0){
            (*pCellValues)[count] = k + 1;
            (*pScores)[count] = sol[indices[index]];
            count++;
        }
    }

    free(indices);
    free(sol);

    *pLength = count;
    realloc1 = (int *)realloc(*pCellValues, count * sizeof(int));

    if (realloc1 == NULL){ /*realloc failed*/
        free(*pCellValues);
        free(*pScores);
        return -2;
    }
    /*realloc succeeded and freed *pCellValues*/
    *pCellValues = realloc1;

    realloc2 = (double *)realloc(*pScores, count * sizeof(double));

    if (realloc2 == NULL){ /*realloc failed*/
        free(*pCellValues);
        free(*pScores);
        return -2;
    }
    /*realloc succeeded and freed *pScores*/
    *pScores = realloc2;

    return 1;
}

/*
 * This method fills availableValues with the legal values for cell (row, col) of which the score is above
 * threshold, and fills their score in the array scores accordingly.
 * It returns its length in *pLength and scores' sum of the available values (the sum of scores array)
 * in *pSumScores.
 */
void createAvailableValues(struct sudokuManager *manager, int *availableValues, float *scores, float threshold,
                           int *indices, double *sol, int *pLength, float *pSumScores, int row, int col, int *retBoard){
    int k, index, N = boardLen(manager);
    *pLength = 0;
    *pSumScores = 0;
    for (k = 0; k < N; k++){
        index = threeDIndex(N, row, col, k);
        if (indices[index] == -1){ /*this value was erroneous from the first place*/
            continue;
        }
        if (sol[indices[index]] >= threshold){ /*score is above the threshold we got*/
            if (!neighbourContainsOnce(retBoard, manager->m, manager->n, row, col, k + 1)){
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
 * retBoard is a copy of manager->board.
* The method returns -1 if Gurobi had an error,
 * -2 if memory allocation failed,
 * 1 if we succeeded in guessing the values (the board is solvable),
 * and 0 if the board is unsolvable.
 * It fills retBoard with the solution guessed iff return value == 1.
*/
int guessSolution(struct sudokuManager *manager,
                  float threshold, int *retBoard) {
    int N = boardLen(manager), amountOfVariables, res;
    int *indices = NULL, *retBoardGurobi = NULL;
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
        free(indices);
        return -2;
    }

    res = solveGurobi(manager, CONTINUOUS, &retBoardGurobi, sol, indices, amountOfVariables); /*running gurobi*/

    if (res == -1 || res == -2 || res == 0){
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
            if (retBoard[matIndex(manager->m, manager->n, i, j)] != 0){
                /*if the cell is not empty, we need to continue to the next cell*/
                continue;
            }
            /*length is the actual length of availableValues and scores */
            /*sumScores is the sum of scores of available values for this cell = (i, j) */
            createAvailableValues(manager, availableValues, scores, threshold,
                                 indices, sol, &length, &sumScores, i, j, retBoard);
            randScore = (((float)(rand())) / RAND_MAX) * sumScores; /*getting a random number between 0 and sumScores*/
            currScore = 0;
            for (k = 0; k < length; k++){
                if ((randScore >= currScore) && (randScore <= scores[k] + currScore)){
                    /*updating the board if the randScore tells us to choose avialableValues[k]*/
                    changeCellValue(retBoard, manager->m, manager->n, i, j, availableValues[k]);
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
    return 1;
}

