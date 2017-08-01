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

void initArgument(ArgumentArray referenceArgument, int functionPosition, char scopeName[]) {

    int isVariablesExists = F;
    int variablePosition = 0;

    for(int x=0; x < referenceArgument.argumentCount; x++) {
        isVariablesExists = F;
        variablePosition = 0;

        isVariablesExists = isVariableExists(&variablePosition, globalFunctionArray.functions[functionPosition].argumentArray.arguments[x].argumentName, scopeName);
        if(!isVariablesExists) {
            variablePosition = globalVariableArray.variableCount;
        }
        //set variable
        strcpy(globalVariableArray.variables[variablePosition].scope_name, scopeName);
        strcpy(globalVariableArray.variables[variablePosition].name, globalFunctionArray.functions[functionPosition].argumentArray.arguments[x].argumentName);            
        globalVariableArray.variables[variablePosition].is_constant = F;
        
        if(referenceArgument.arguments[x].argumentType == arg_integer_type) {
            globalVariableArray.variables[variablePosition].variable_type = var_integer_type;
            globalVariableArray.variables[variablePosition].integer_value = referenceArgument.arguments[x].integer_value;
        } else if(referenceArgument.arguments[x].argumentType == arg_float_type) {
            globalVariableArray.variables[variablePosition].variable_type = var_float_type;
            globalVariableArray.variables[variablePosition].float_value = referenceArgument.arguments[x].float_value;
        } else if(referenceArgument.arguments[x].argumentType == arg_string_type) {
            globalVariableArray.variables[variablePosition].variable_type = var_string_type;
            strcpy(globalVariableArray.variables[variablePosition].string_value,referenceArgument.arguments[x].string_value);
        } else {
            //arg_none_type
            globalVariableArray.variables[variablePosition].variable_type = var_none_type;
        }   

        if(!isVariablesExists) {
            globalVariableArray.variableCount += 1;
        }
    }
}

void cleanVariable() {
    int counter = 0;
    for(int x=0; x < globalVariableArray.variableCount; x++) {
        if(!strcmp(globalVariableArray.variables[x].scope_name, TITIK_MAIN_SCOPE_NAME)) {

            if(x != counter) {
                strcpy(globalVariableArray.variables[counter].scope_name, TITIK_MAIN_SCOPE_NAME);
                strcpy(globalVariableArray.variables[counter].name, globalVariableArray.variables[x].name);            
                globalVariableArray.variables[counter].is_constant = globalVariableArray.variables[x].is_constant;
                globalVariableArray.variables[counter].variable_type = globalVariableArray.variables[x].variable_type;
                
                if(globalVariableArray.variables[x].variable_type == var_integer_type) {
                    globalVariableArray.variables[counter].integer_value = globalVariableArray.variables[x].integer_value;
                } else if(globalVariableArray.variables[x].variable_type == var_float_type) {
                    globalVariableArray.variables[counter].float_value = globalVariableArray.variables[x].float_value;
                } else if(globalVariableArray.variables[x].variable_type == var_string_type) {
                    strcpy(globalVariableArray.variables[counter].string_value,globalVariableArray.variables[x].string_value);
                }                
            }

            counter += 1;
        }        
    }

    globalVariableArray.variableCount = counter;
}

void copyVariable(char toScope[]) {
    for(int x=0; x < globalVariableArray.variableCount; x++) {
        if(!strcmp(globalVariableArray.variables[x].scope_name, TITIK_MAIN_SCOPE_NAME)) {
            strcpy(globalVariableArray.variables[globalVariableArray.variableCount].scope_name, toScope);
            strcpy(globalVariableArray.variables[globalVariableArray.variableCount].name, globalVariableArray.variables[x].name);            
            globalVariableArray.variables[globalVariableArray.variableCount].is_constant = globalVariableArray.variables[x].is_constant;
            globalVariableArray.variables[globalVariableArray.variableCount].variable_type = globalVariableArray.variables[x].variable_type;
            
            if(globalVariableArray.variables[x].variable_type == var_integer_type) {
                globalVariableArray.variables[globalVariableArray.variableCount].integer_value = globalVariableArray.variables[x].integer_value;
            } else if(globalVariableArray.variables[x].variable_type == var_float_type) {
                globalVariableArray.variables[globalVariableArray.variableCount].float_value = globalVariableArray.variables[x].float_value;
            } else if(globalVariableArray.variables[x].variable_type == var_string_type) {
                strcpy(globalVariableArray.variables[globalVariableArray.variableCount].string_value,globalVariableArray.variables[x].string_value);
            }

            globalVariableArray.variableCount += 1;
        }
    }
}

void setVariableInfo(int variablePosition, Token currentIdentifier, char currentScope[]) {
    //set name & scope
    strcpy(globalVariableArray.variables[variablePosition].name, currentIdentifier.tokenValue);
    strcpy(globalVariableArray.variables[variablePosition].scope_name, currentScope);

    //set constant or not
    //if the first letter is capital then it's a constant else it's not
    if(isupper(globalVariableArray.variables[variablePosition].name[0])) {
        globalVariableArray.variables[variablePosition].is_constant = T;
    } else {
        globalVariableArray.variables[variablePosition].is_constant = F;
    }
}

int evaluateToken(Token currentIdentifier, int * ifWithTrue, char currentScope[]) {
    int ret = 0;
    int isVariablesExists = F;
    int variablePosition2 = 0;

    switch(currentIdentifier.tokenType) {
        case identifier_token:

            isVariablesExists = isVariableExists(&variablePosition2, currentIdentifier.tokenValue, currentScope);
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

void compareVariable(Variable tempVariable, Variable tempVariable2, TitikTokenType currentOperation, int * ifWithTrue){
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

int convertTokenToVariable(Variable * tempVariable, Token token, char currentScope[]) {
    int ret = 0;
    int isVariablesExists = F;
    int variablePosition = 0;

    switch(token.tokenType) {
        case identifier_token:
            isVariablesExists = isVariableExists(&variablePosition, token.tokenValue, currentScope);
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

void defineFunction(char functionName[], ArgumentArray argumentArray, void(*execute)(ArgumentArray argumentArray,  int * intReturn, FunctionReturn * funcReturn), int isSystem) {
    strcpy(globalFunctionArray.functions[globalFunctionArray.functionCount].functionName, functionName);

    globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.argumentCount = 0;
    globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    //add arguments structure here
    
    for(int x=0; x<argumentArray.argumentCount; x++) {
        
        globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.arguments[x] = argumentArray.arguments[x];
        globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.argumentCount += 1;
    }

    globalFunctionArray.functions[globalFunctionArray.functionCount].execute = execute;
    globalFunctionArray.functions[globalFunctionArray.functionCount].isSystem = isSystem;
    //last statement to be executed
    globalFunctionArray.functionCount += 1;
}

void p_execute(ArgumentArray argumentArray,  int * intReturn, FunctionReturn * funcReturn) {
    switch(argumentArray.arguments[0].argumentType) {
        case arg_string_type:
            printf("%s\n", escape_string(argumentArray.arguments[0].string_value));
        break;
        case arg_integer_type:
            printf("%ld\n", argumentArray.arguments[0].integer_value);
        break;
        case arg_float_type:
            printf("%f\n", argumentArray.arguments[0].float_value);
        break;
        default:
            printf("Nil\n");
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
    FunctionReturn thisReturn;
    TokenArray tokenArray;
    tokenArray.tokens = malloc(TITIK_TOKEN_INIT_LENGTH * sizeof(Token));
    tokenArray.tokenCount = 0;
    *intReturn = 0;
    int gotReturn = F;

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
                functionReturn = parseToken(tokenArray, F, T, &needBreak, TITIK_MAIN_SCOPE_NAME, &thisReturn, &gotReturn);
            }
        }
    } 

    funcReturn->returnType = ret_none_type;
    free(tokenArray.tokens);     
}


void r_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {

    char userInput[TITIK_CHAR_PER_LINE];
    char tempChar[TITIK_CHAR_PER_LINE];
    strcpy(funcReturn->string_value, "");
    switch(argumentArray.arguments[0].argumentType) {
        case arg_string_type:
            printf("%s", escape_string(argumentArray.arguments[0].string_value));
        break;
        case arg_integer_type:
            printf("%ld", argumentArray.arguments[0].integer_value);
        break;
        case arg_float_type:
            printf("%f", argumentArray.arguments[0].float_value);
        break;
        default:
            printf("Nil");
    }

    fgets(userInput, TITIK_CHAR_PER_LINE, stdin);

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
    *intReturn = 0;
    if(argumentArray.arguments[0].argumentType != arg_integer_type) {
        *intReturn = 1;
        printf("Error: Parameter must be an integer\n");
    }

    #ifdef _WIN32
    Sleep(argumentArray.arguments[0].integer_value);
    #elif __linux__
    sleep(argumentArray.arguments[0].integer_value / 1000);
    #else
    usleep(argumentArray.arguments[0].integer_value * 1000);
    #endif

    funcReturn->returnType = ret_none_type;
}

void toi_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;

    switch(argumentArray.arguments[0].argumentType){
        case arg_string_type:
            funcReturn->integer_value = atoi(argumentArray.arguments[0].string_value);
        break;
        case arg_float_type:
            funcReturn->integer_value = (long int)argumentArray.arguments[0].float_value;
        break;
        case arg_integer_type:
            funcReturn->integer_value = argumentArray.arguments[0].integer_value;
        break;
        default:
            *intReturn = 1;
            printf("Error: Can't convert Nil type to integer\n");
    }

    //funcReturn->integer_value = 0;
    funcReturn->returnType = ret_integer_type;
}

void tof_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;

    switch(argumentArray.arguments[0].argumentType){
        case arg_string_type:
            funcReturn->float_value = atof(argumentArray.arguments[0].string_value);
        break;
        case arg_float_type:
            funcReturn->float_value = argumentArray.arguments[0].float_value;
        break;
        case arg_integer_type:
            funcReturn->float_value = (double)argumentArray.arguments[0].integer_value;
        break;
        default:
            *intReturn = 1;
            printf("Error: Can't convert Nil type to float\n");
    }

    funcReturn->returnType = ret_float_type;
}

void tos_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    char tempChar[TITIK_CHAR_PER_LINE];

    switch(argumentArray.arguments[0].argumentType){
        case arg_string_type:
            strcpy(funcReturn->string_value, argumentArray.arguments[0].string_value);
        break;
        case arg_float_type:
            snprintf(tempChar, TITIK_CHAR_PER_LINE, "%f",argumentArray.arguments[0].float_value);
            strcpy(funcReturn->string_value, tempChar);
        break;
        case arg_integer_type:
            snprintf(tempChar, TITIK_CHAR_PER_LINE, "%ld",argumentArray.arguments[0].integer_value);
            strcpy(funcReturn->string_value, tempChar);
        break;
        default:
            strcpy(funcReturn->string_value, "Nil");
    }

    funcReturn->returnType = ret_string_type;
}

void ex_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    if(argumentArray.arguments[0].argumentType != arg_integer_type) {
        *intReturn = 1;
        printf("Error: Parameter must be an integer\n");
    }

    exit(argumentArray.arguments[0].integer_value);

    funcReturn->returnType = ret_none_type;
}

void flcp_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    FILE *source, *target;
    char ch;
    *intReturn = 0;
    funcReturn->returnType = ret_string_type;
    strcpy(funcReturn->string_value, "");

    if(argumentArray.arguments[0].argumentType != arg_integer_type) {
        *intReturn = 1;
        printf("Error: Parameter must be a string\n");
    }

    if(argumentArray.arguments[1].argumentType != arg_integer_type) {
        *intReturn = 1;
        printf("Error: Parameter must be a string\n");
    }
}

#ifdef _WIN32
CONSOLE_SCREEN_BUFFER_INFO Info;
#endif
void sc_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    if(argumentArray.arguments[0].argumentType != arg_integer_type) {
        *intReturn = 1;
        printf("Error: Parameter must be an integer\n");
    }

    #ifdef _WIN32
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    switch(argumentArray.arguments[0].integer_value) {
        case 0:
            //SetConsoleTextAttribute(hConsole, TITIK_COLOR_NORMAL);
            SetConsoleTextAttribute(hConsole, Info.wAttributes);
        break;
        case 1:
            SetConsoleTextAttribute(hConsole, TITIK_COLOR_RED);
        break;
        case 2:
            SetConsoleTextAttribute(hConsole, TITIK_COLOR_GREEN);
        break;
        case 3:
            SetConsoleTextAttribute(hConsole, TITIK_COLOR_YELLOW);
        break;
        case 4:
            SetConsoleTextAttribute(hConsole, TITIK_COLOR_BLUE);
        break;
        case 5:
            SetConsoleTextAttribute(hConsole, TITIK_COLOR_MAGENTA);
        break;
        case 6:
            SetConsoleTextAttribute(hConsole, TITIK_COLOR_CYAN);
        break;
        case 7:
            SetConsoleTextAttribute(hConsole, TITIK_COLOR_WHITE);
        break;
        default:
            *intReturn = 1;
            printf("Error: Invalid value\n");
    }
    #else
    switch(argumentArray.arguments[0].integer_value) {
        case 0:
            printf(TITIK_COLOR_NORMAL);
        break;
        case 1:
            printf(TITIK_COLOR_RED);
        break;
        case 2:
            printf(TITIK_COLOR_GREEN);
        break;
        case 3:
            printf(TITIK_COLOR_YELLOW);
        break;
        case 4:
            printf(TITIK_COLOR_BLUE);
        break;
        case 5:
            printf(TITIK_COLOR_MAGENTA);
        break;
        case 6:
            printf(TITIK_COLOR_CYAN);
        break;
        case 7:
            printf(TITIK_COLOR_WHITE);
        break;
        default:
            *intReturn = 1;
            printf("Error: Invalid value\n");
    }
    #endif
    funcReturn->returnType = ret_none_type;
}

void initFunctions() {

    //p function
    ArgumentArray pArgArray;
    pArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    pArgArray.argumentCount = 1; //set number of args
    pArgArray.arguments[0].argumentType = arg_string_type;
    strcpy(pArgArray.arguments[0].string_value, "");
    defineFunction("p", pArgArray, p_execute, T);
    //end p function

    //i function
    ArgumentArray iArgArray;
    iArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    iArgArray.argumentCount = 1; //set number of args
    iArgArray.arguments[0].argumentType = arg_string_type;
    strcpy(iArgArray.arguments[0].string_value, "");
    defineFunction("i", iArgArray, i_execute, T);
    //end i function

    //r function
    ArgumentArray rArgArray;
    rArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    rArgArray.argumentCount = 1; //set number of args
    rArgArray.arguments[0].argumentType = arg_string_type;
    strcpy(rArgArray.arguments[0].string_value, "");
    defineFunction("r", rArgArray, r_execute, T);
    //end r function

    //zzz function
    ArgumentArray zzzArgArray;
    zzzArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    zzzArgArray.argumentCount = 1; //set number of args
    zzzArgArray.arguments[0].argumentType = arg_integer_type;
    zzzArgArray.arguments[0].integer_value = 0;
    defineFunction("zzz", zzzArgArray, zzz_execute, T);
    //end zzz function

    //toi function
    ArgumentArray toiArgArray;
    toiArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    toiArgArray.argumentCount = 1;
    defineFunction("toi", toiArgArray, toi_execute, T);
    //end toi function

    //tof function
    ArgumentArray tofArgArray;
    tofArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    tofArgArray.argumentCount = 1;
    defineFunction("tof", tofArgArray, tof_execute, T);
    //end tof function

    //tos function
    ArgumentArray tosArgArray;
    tosArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    tosArgArray.argumentCount = 1;
    defineFunction("tos", tosArgArray, tos_execute, T);
    //end tos function

    //ex function
    ArgumentArray exArgArray;
    exArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    exArgArray.argumentCount = 1;
    defineFunction("ex", exArgArray, ex_execute, T);
    //end ex function

    //sc function
    ArgumentArray scArgArray;
    scArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    scArgArray.argumentCount = 1;
    defineFunction("sc", scArgArray, sc_execute, T);
    //end sc function

    //flcp function
    ArgumentArray flcpArgArray;
    flcpArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    flcpArgArray.argumentCount = 2;
    defineFunction("flcp", flcpArgArray, flcp_execute, T);
    //end flcp function
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

int isFunctionArgumentExists(ArgumentArray argArray, int * argumentPosition, char tokenValue[]) {
    int isExists = F;
    int strCompRet = 0;

    for(int x2=0;x2 < argArray.argumentCount; x2++) {
        strCompRet = strcmp(tokenValue, argArray.arguments[x2].argumentName);

        if(!(strCompRet < 0) && !(strCompRet > 0)) {
            isExists = T;
            *argumentPosition = x2;
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

void defineConstantInteger(char variableName[], int variableValue) {
    globalVariableArray.variables[globalVariableArray.variableCount].variable_type = var_integer_type;
    strcpy(globalVariableArray.variables[globalVariableArray.variableCount].name, variableName);
    strcpy(globalVariableArray.variables[globalVariableArray.variableCount].scope_name, TITIK_MAIN_SCOPE_NAME);
    globalVariableArray.variables[globalVariableArray.variableCount].integer_value = variableValue;
    globalVariableArray.variables[globalVariableArray.variableCount].is_constant = T;
    globalVariableArray.variableCount += 1;
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
    //colors
    defineConstantInteger("CNRM", 0);
    defineConstantInteger("CRED", 1);
    defineConstantInteger("CGRN", 2);
    defineConstantInteger("CYLW", 3);
    defineConstantInteger("CBLU", 4);
    defineConstantInteger("CMAG", 5);
    defineConstantInteger("CCYN", 6);
    defineConstantInteger("CWHT", 7);
    //end of colors
    defineConstantNone("Nil");
}