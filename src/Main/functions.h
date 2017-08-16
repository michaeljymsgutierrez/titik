/* functions.h
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#define TITIK_VARIABLE_INIT_LENGTH 10000
#define TITIK_FUNCTION_INIT_LENGTH 10000
#define TITIK_ARGUMENT_INIT_LENGTH 10000
#define TITIK_MAIN_SCOPE_NAME "main"
#include "util.h"
#include "lexer.h"

typedef enum {
    var_none_type,
    var_integer_type,
    var_string_type,
    var_float_type,
    var_array_type
} variable_type;

static const char *variableTypeString[] = {
    "none_type",
    "integer_type",
    "string_type",
    "float_type",
    "array_type"
};

typedef struct _Variable{
    char name[TITIK_CHAR_PER_LINE];
    char scope_name[TITIK_CHAR_PER_LINE];
    variable_type variable_type;
    char string_value[TITIK_CHAR_PER_LINE];
    long int integer_value;
    double float_value;
    int is_constant;
    int array_count;
    int array_init;
    struct _Variable* array_value;
} Variable;

typedef struct {

    Variable* variables;
    int variableCount;

} VariableArray;

typedef enum {
    ret_string_type,
    ret_integer_type,
    ret_float_type,
    ret_array_type,
    ret_none_type
} FunctionReturnType;

typedef struct {
    //char returnValue[TITIK_CHAR_PER_LINE];
    char string_value[TITIK_CHAR_PER_LINE];
    long int integer_value;
    double float_value;    
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
    char argumentName[TITIK_CHAR_PER_LINE];
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
    int isSystem; //tells if the function is user defined or not
    void (*execute)(ArgumentArray argumentArray2, int * intReturn, FunctionReturn * funcReturn);
    TokenArray tokens;
    FunctionReturn functionReturn;
} Function;

typedef struct {
    Function* functions;
    int functionCount;
} FunctionArray;

#ifdef __linux__
//dirty fix for the warning in linux
int setenv(const char * envname, const char * envval, int overwrite);
#endif

int addArrayItem(Token token, int variablePosition, char scopeName[]);
void initArgument(ArgumentArray referenceArgument, int functionPosition, char scopeName[]);
void cleanVariable();
void copyVariable(char toScope[]);
void setVariableInfo(int variablePosition, Token currentIdentifier, char currentScope[]);
int evaluateToken(Token currentIdentifier, int * ifWithTrue, char currentScope[]);
void compareVariable(Variable tempVariable, Variable tempVariable2, TitikTokenType currentOperation, int * ifWithTrue);
int convertTokenToVariable(Variable * tempVariable, Token token, char currentScope[]);
void p_execute(ArgumentArray argumentArray2, int * intReturn, FunctionReturn * funcReturn);
void i_execute(ArgumentArray argumentArray2, int * intReturn, FunctionReturn * funcReturn);
void r_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void zzz_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void toi_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void tof_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void tos_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void ex_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
int cpmv(char sourceFile[], char targetFile[], int removeIt);
void flcp_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void flmv_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void flrm_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void exe_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void sc_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void rnd_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void sac_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void savf_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void rndstr_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void senv_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
void genv_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn);
//the first argument array is the structure of the function
//the function pointer with argumentarray is the user inputted argument
void defineFunction(char functionName[], ArgumentArray argumentArray, void(*execute)(ArgumentArray argumentArray2, int * intReturn, FunctionReturn * funcReturn), int isSystem);
void initFunctions();
int isFunctionExists(int * functionPosition, char tokenValue[]);
int isFunctionArgumentExists(ArgumentArray argArray, int * argumentPosition, char tokenValue[]);
int isVariableExists(int * variablePosition, char tokenValue[], char scopeName[]);

void defineConstantInteger(char variableName[], int variableValue);
void defineConstantString(char variableName[], char variableValue[]);
void defineConstantNone(char variableName[]);
void initVariables();
#endif