/* functions.h
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#define TITIK_VARIABLE_INIT_LENGTH 50000
#define TITIK_FUNCTION_INIT_LENGTH 50000
#define TITIK_ARGUMENT_INIT_LENGTH 50000
#define TITIK_MAIN_SCOPE_NAME "main"
#include "util.h"

typedef enum {
    var_none_type,
    var_integer_type,
    var_string_type,
    var_float_type
    //array_type
} variable_type;

static const char *variableTypeString[] = {
    "none_type",
    "integer_type",
    "string_type",
    "float_type"
};

typedef struct _Variable{
    char name[TITIK_VARIABLE_INIT_LENGTH];
    char scope_name[TITIK_CHAR_PER_LINE];
    variable_type variable_type;
    char string_value[TITIK_VARIABLE_INIT_LENGTH];
    long int integer_value;
    double float_value;
    int is_constant;
    //int array_count;
    //struct _Variable* array_value;
} Variable;

typedef struct {

    Variable* variables;
    int variableCount;

} VariableArray;

typedef enum {
    ret_string_type,
    ret_integer_type,
    ret_float_type,
    ret_none_type
} FunctionReturnType;

typedef struct {
    char returnValue[TITIK_CHAR_PER_LINE];
    FunctionReturnType returnType;
} FunctionReturn;

typedef enum {
    arg_string_type,
    arg_integer_type,
    arg_float_type,
    arg_none_type
    //variable_type HERE <<--- ?
} ArgumentType;

typedef struct {
    char string_value[TITIK_CHAR_PER_LINE];
    long int integer_value;
    double float_value;
    ArgumentType argumentType;
} Argument;

typedef struct {
    Argument* arguments;
    int argumentCount;
} ArgumentArray;

typedef struct {
    ArgumentArray argumentArray;
    char functionName[TITIK_CHAR_PER_LINE];
    void (*execute)(ArgumentArray argumentArray2, VariableArray * variableArray, int * intReturn);
} Function;

typedef struct {
    Function* functions;
    int functionCount;
} FunctionArray;

void p_execute(ArgumentArray argumentArray2, VariableArray * variableArray, int * intReturn);
void i_execute(ArgumentArray argumentArray2, VariableArray * variableArray, int * intReturn);
//the first argument array is the structure of the function
//the function pointer with argumentarray is the user inputted argument
void defineFunction(char functionName[], ArgumentArray argumentArray, void(*execute)(ArgumentArray argumentArray2, VariableArray * variableArray, int * intReturn), FunctionArray * functionArray);
void initFunctions(FunctionArray * functionArray);
int isFunctionExists(FunctionArray functionArray, int * functionPosition, char tokenValue[]);
int isVariableExists(VariableArray variableArray, int * variablePosition, char tokenValue[], char scopeName[]);

void defineConstantString(VariableArray * variableArray, char variableName[], char variableValue[]);
void defineConstantNone(VariableArray * variableArray, char variableName[]);
void initVariables(VariableArray * variableArray);
#endif