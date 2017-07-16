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
#include <ctype.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

extern FunctionArray globalFunctionArray;
extern VariableArray globalVariableArray;

void setVariableInfo(int variablePosition, Token currentIdentifier) {
    //set name & scope
    strcpy(globalVariableArray.variables[variablePosition].name, currentIdentifier.tokenValue);
    strcpy(globalVariableArray.variables[variablePosition].scope_name, TITIK_MAIN_SCOPE_NAME);

    //set constant or not
    //if the first letter is capital then it's a constant else it's not
    if(isupper(globalVariableArray.variables[variablePosition].name[0])) {
        globalVariableArray.variables[variablePosition].is_constant = T;
    } else {
        globalVariableArray.variables[variablePosition].is_constant = F;
    }
}

int evaluateToken(Token currentIdentifier, int * ifWithTrue) {
    int ret = 0;
    int isVariablesExists = F;
    int variablePosition2 = 0;

    switch(currentIdentifier.tokenType) {
        case identifier_token:

            isVariablesExists = isVariableExists(&variablePosition2, currentIdentifier.tokenValue, TITIK_MAIN_SCOPE_NAME);
            if(!isVariablesExists) {
                return unexpected_error(currentIdentifier.tokenLine, currentIdentifier.tokenColumn, "Undefined variable ", currentIdentifier.tokenValue, currentIdentifier.fileName);
            }

            switch(globalVariableArray.variables[variablePosition2].variable_type) {
                case var_string_type:
                    *ifWithTrue = strcmp(globalVariableArray.variables[variablePosition2].string_value, "")?T:F;
                break;
                case var_float_type:
                    *ifWithTrue = globalVariableArray.variables[variablePosition2].float_value?T:F;
                break;
                case var_integer_type:
                    *ifWithTrue = globalVariableArray.variables[variablePosition2].integer_value?T:F;
                break;
                default:
                    *ifWithTrue = F;
            }

        break;
        case string_token:
            *ifWithTrue = strcmp(currentIdentifier.tokenValue, "")?T:F;
        break;
        case integer_token:
            *ifWithTrue = atoi(currentIdentifier.tokenValue)?T:F;
        break;
        case float_token:
            *ifWithTrue = atof(currentIdentifier.tokenValue)?T:F;
        break;
        default:
            return unexpected_error(currentIdentifier.tokenLine, currentIdentifier.tokenColumn, "Unexpected token ", currentIdentifier.tokenValue, currentIdentifier.fileName);
    }

    return ret;
}

void compareVariable(Variable tempVariable, Variable tempVariable2, TokenType currentOperation, int * ifWithTrue){
    switch(tempVariable.variable_type) {
        case var_string_type:
            if(currentOperation == equals_token) {
                if(tempVariable2.variable_type == var_string_type) {
                    *ifWithTrue = !strcmp(tempVariable.string_value, tempVariable2.string_value)?T:F;
                }
            }
        break;
        case var_integer_type:
            if(currentOperation == greater_than_token) {
                if(tempVariable2.variable_type == var_integer_type) {
                    *ifWithTrue = (tempVariable.integer_value > tempVariable2.integer_value)?T:F;
                }
                if(tempVariable2.variable_type == var_float_type) {
                    *ifWithTrue = (tempVariable.integer_value > tempVariable2.float_value)?T:F;
                }
            } else if(currentOperation == less_than_token) {
                if(tempVariable2.variable_type == var_integer_type) {
                    *ifWithTrue = (tempVariable.integer_value < tempVariable2.integer_value)?T:F;
                }
                if(tempVariable2.variable_type == var_float_type) {
                    *ifWithTrue = (tempVariable.integer_value < tempVariable2.float_value)?T:F;
                }
            } else {
                //equals
                if(tempVariable2.variable_type == var_integer_type) {
                    *ifWithTrue = (tempVariable.integer_value == tempVariable2.integer_value)?T:F;
                }
                if(tempVariable2.variable_type == var_float_type) {
                    *ifWithTrue = (tempVariable.integer_value == tempVariable2.float_value)?T:F;
                }
            }
        break;
        case var_float_type:
            if(currentOperation == greater_than_token) {
                if(tempVariable2.variable_type == var_integer_type) {
                    *ifWithTrue = (tempVariable.float_value > tempVariable2.integer_value)?T:F;
                }
                if(tempVariable2.variable_type == var_float_type) {
                    *ifWithTrue = (tempVariable.float_value > tempVariable2.float_value)?T:F;
                }
            } else if(currentOperation == less_than_token) {
                if(tempVariable2.variable_type == var_integer_type) {
                    *ifWithTrue = (tempVariable.float_value < tempVariable2.integer_value)?T:F;
                }
                if(tempVariable2.variable_type == var_float_type) {
                    *ifWithTrue = (tempVariable.float_value < tempVariable2.float_value)?T:F;
                }
            } else {
                //equals
                if(tempVariable2.variable_type == var_integer_type) {
                    *ifWithTrue = (tempVariable.float_value == tempVariable2.integer_value)?T:F;
                }
                if(tempVariable2.variable_type == var_float_type) {
                    *ifWithTrue = (tempVariable.float_value == tempVariable2.float_value)?T:F;
                }
            }
        break;
        default:
            //none type
            if(currentOperation == equals_token) {
                if(tempVariable2.variable_type == var_none_type) {
                    *ifWithTrue = T;
                }
            }
    }
}

int convertTokenToVariable(Variable * tempVariable, Token token) {
    int ret = 0;
    int isVariablesExists = F;
    int variablePosition = 0;

    switch(token.tokenType) {
        case identifier_token:
            isVariablesExists = isVariableExists(&variablePosition, token.tokenValue, TITIK_MAIN_SCOPE_NAME);
            if(!isVariablesExists) {
                return unexpected_error(token.tokenLine, token.tokenColumn, "Undefined variable ", token.tokenValue, token.fileName);
            }

            *tempVariable = globalVariableArray.variables[variablePosition];
        break;
        case string_token:
            tempVariable->variable_type = var_string_type;
            strcpy(tempVariable->string_value, token.tokenValue);
        break;
        case integer_token:
            tempVariable->variable_type = var_integer_type;
            tempVariable->integer_value = atoi(token.tokenValue);
        break;
        case float_token:
            tempVariable->variable_type = var_float_type;
            tempVariable->float_value = atof(token.tokenValue);
        break;
        default:
            return unexpected_error(token.tokenLine, token.tokenColumn, "Unexpected token ", token.tokenValue, token.fileName);
    }

    return ret;
}

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
    int needBreak = F;
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
                functionReturn = parseToken(tokenArray, F, &needBreak);
            }
        }
    } 

    funcReturn->returnType = ret_none_type;
    free(tokenArray.tokens);     
}


void r_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {

    char userInput[1000];
    char tempChar[1000];
    strcpy(funcReturn->string_value, "");
    switch(argumentArray.arguments[0].argumentType) {
        case arg_string_type:
            printf("%s", argumentArray.arguments[0].string_value);
        break;
        case arg_integer_type:
            printf("%ld", argumentArray.arguments[0].integer_value);
        break;
        case arg_float_type:
            printf("%f", argumentArray.arguments[0].float_value);
        break;
        default:
            printf("None");
    }

    fgets(userInput, 1000, stdin);

    memset(tempChar, 0, strlen(tempChar));
    //cleanup new line
    for(int x=0; x < strlen(userInput); x++) {
        if((x+1) == strlen(userInput)) {
            tempChar[x] = 0;
        } else {
            tempChar[x] = userInput[x];
        }
    }

    strcpy(funcReturn->string_value, tempChar);
    
    funcReturn->returnType = ret_string_type;
    *intReturn = 0;
}

void zzz_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {

    if(argumentArray.arguments[0].argumentType != arg_integer_type) {
        *intReturn = 1;
        printf("Error: Parameter must be an integer\n");
    }

    #ifdef _WIN32
    Sleep(argumentArray.arguments[0].integer_value);
    #else
    usleep(argumentArray.arguments[0].integer_value * 1000);
    #endif

    funcReturn->returnType = ret_none_type;
    *intReturn = 0;
}

void initFunctions() {

    //p function
    ArgumentArray pArgArray;
    pArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    pArgArray.argumentCount = 1; //set number of args
    pArgArray.arguments[0].argumentType = arg_string_type;
    strcpy(pArgArray.arguments[0].string_value, "");
    defineFunction("p", pArgArray, p_execute);
    //end p function

    //i function
    ArgumentArray iArgArray;
    iArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    iArgArray.argumentCount = 1; //set number of args
    iArgArray.arguments[0].argumentType = arg_string_type;
    strcpy(iArgArray.arguments[0].string_value, "");
    defineFunction("i", iArgArray, i_execute);
    //end i function

    //r function
    ArgumentArray rArgArray;
    rArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    rArgArray.argumentCount = 1; //set number of args
    rArgArray.arguments[0].argumentType = arg_string_type;
    strcpy(rArgArray.arguments[0].string_value, "");
    defineFunction("r", rArgArray, r_execute);
    //end r function

    //zzz function
    ArgumentArray zzzArgArray;
    zzzArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    zzzArgArray.argumentCount = 1; //set number of args
    zzzArgArray.arguments[0].argumentType = arg_integer_type;
    zzzArgArray.arguments[0].integer_value = 0;
    defineFunction("zzz", zzzArgArray, zzz_execute);
    //end zzz function
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