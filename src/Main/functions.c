/* functions.c
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#include "functions.h"
#include "util.h"
#include "info.h"
#include "lexer.h"
#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern FunctionArray globalFunctionArray;
extern VariableArray globalVariableArray;

void defineFunction(char functionName[], ArgumentArray argumentArray, void(*execute)(ArgumentArray argumentArray,  int * intReturn, FunctionReturn * funcReturn)) {
    strcpy(globalFunctionArray.functions[globalFunctionArray.functionCount].functionName, functionName);

    globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.argumentCount = 0;
    globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    //add arguments structure here
    
    for(int x=0; x<argumentArray.argumentCount; x++) {
        
        globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.arguments[x] = argumentArray.arguments[x];
        globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.argumentCount += 1;
    }

    globalFunctionArray.functions[globalFunctionArray.functionCount].execute = execute;
    //last statement to be executed
    globalFunctionArray.functionCount += 1;
}

void p_execute(ArgumentArray argumentArray,  int * intReturn, FunctionReturn * funcReturn) {
    switch(argumentArray.arguments[0].argumentType) {
        case arg_string_type:
            printf("%s\n", argumentArray.arguments[0].string_value);
        break;
        case arg_integer_type:
            printf("%ld\n", argumentArray.arguments[0].integer_value);
        break;
        case arg_float_type:
            printf("%f\n", argumentArray.arguments[0].float_value);
        break;
        default:
            printf("None\n");
    }

    funcReturn->returnType = ret_none_type;
    *intReturn = 0;
}

void i_execute(ArgumentArray argumentArray2,  int * intReturn, FunctionReturn * funcReturn) {
    char **fileContent;
    int functionReturn;
    int totalLineCount;
    char fName[TITIK_CHAR_PER_LINE];
    int fileExists = 0;
    TokenArray tokenArray;
    tokenArray.tokens = malloc(TITIK_TOKEN_INIT_LENGTH * sizeof(Token));
    tokenArray.tokenCount = 0;
    *intReturn = 0;

    strcpy(fName, argumentArray2.arguments[0].string_value);
    strcat(fName, ".ttk");

    fileExists = isFileExists(fName);

    if(fileExists) {
        *intReturn = fileExists;
        printf("Error: File '%s' already loaded\n", fName);
    } else {
        fileContent = readSourceFile(fName, &functionReturn, &totalLineCount);

        if(functionReturn > 0) {
            *intReturn = functionReturn;
        } else {
            //generate token
            functionReturn = generateToken(fileContent, totalLineCount, &tokenArray, fName);

            if(functionReturn > 0) {
                *intReturn = functionReturn;
            } else {
                //parse token
                functionReturn = parseToken(tokenArray, &globalFunctionArray);
            }
        }
    } 

    funcReturn->returnType = ret_none_type;
    free(tokenArray.tokens);     
}

void initFunctions() {

    //p function
    ArgumentArray pArgArray;
    pArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    pArgArray.argumentCount = 0;
    pArgArray.argumentCount += 1;
    pArgArray.arguments[0].argumentType = arg_string_type;
    strcpy(pArgArray.arguments[0].string_value, "");

    defineFunction("p", pArgArray, p_execute);
    //end p function

    //i function
    ArgumentArray iArgArray;
    iArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    iArgArray.argumentCount = 0;
    iArgArray.argumentCount += 1;
    iArgArray.arguments[0].argumentType = arg_string_type;
    strcpy(iArgArray.arguments[0].string_value, "");

    defineFunction("i", iArgArray, i_execute);
    //end i function

}

int isFunctionExists(int * functionPosition, char tokenValue[]) {
    int isExists = F;
    int strCompRet = 0;

    for(int x2=0;x2 < globalFunctionArray.functionCount; x2++) {
        strCompRet = strcmp(tokenValue, globalFunctionArray.functions[x2].functionName);

        if(!(strCompRet < 0) && !(strCompRet > 0)) {
            isExists = T;
            *functionPosition = x2;
            break;
        }
    }

    return isExists;
}

int isVariableExists(int * variablePosition, char tokenValue[], char scopeName[]) {
    int isExists = F;
    int strCompRet = 0;
    int strCompRet2 = 0;

    for(int x2=0;x2 < globalVariableArray.variableCount; x2++) {
        strCompRet = strcmp(tokenValue, globalVariableArray.variables[x2].name);
        strCompRet2 = strcmp(scopeName, globalVariableArray.variables[x2].scope_name);

        if((!(strCompRet < 0) && !(strCompRet > 0)) && (!(strCompRet2 < 0) && !(strCompRet2 > 0))) {
            isExists = T;
            *variablePosition = x2;
            break;
        }
    }

    return isExists;    
}

void defineConstantString(char variableName[], char variableValue[]) {
    globalVariableArray.variables[globalVariableArray.variableCount].variable_type = var_string_type;
    strcpy(globalVariableArray.variables[globalVariableArray.variableCount].name, variableName);
    strcpy(globalVariableArray.variables[globalVariableArray.variableCount].scope_name, TITIK_MAIN_SCOPE_NAME);
    strcpy(globalVariableArray.variables[globalVariableArray.variableCount].string_value, variableValue);
    globalVariableArray.variables[globalVariableArray.variableCount].is_constant = T;
    globalVariableArray.variableCount += 1;
}

void defineConstantNone(char variableName[]) {
    globalVariableArray.variables[globalVariableArray.variableCount].variable_type = var_none_type;
    strcpy(globalVariableArray.variables[globalVariableArray.variableCount].name, variableName);
    strcpy(globalVariableArray.variables[globalVariableArray.variableCount].scope_name, TITIK_MAIN_SCOPE_NAME);
    globalVariableArray.variables[globalVariableArray.variableCount].is_constant = T;
    globalVariableArray.variableCount += 1;
}

void initVariables() {
    defineConstantString("__AUTHOR__", TITIK_AUTHOR);
    defineConstantString("__VERSION_STRING__", TITIK_STRING_VERSION);
    defineConstantNone("None");
}