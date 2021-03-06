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
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#ifdef T_W_MYSQL
#include <mysql.h>
extern MYSQL *globalMySQLConnection;
extern int globalMySQLIsConnected;
#endif

extern FunctionArray globalFunctionArray;
extern VariableArray globalVariableArray;
extern FunctionReturnArray globalFunctionReturnArray;
extern int globalArgC;
extern char **globalArgV;

void setArrayReturnToVariable(int lastVariablePosition) {
    for(int x=0; x < globalFunctionReturnArray.functionReturnCount; x++) {
        
        if(globalFunctionReturnArray.functionReturns[x].returnType == ret_float_type) {
            globalVariableArray.variables[lastVariablePosition].array_value[globalVariableArray.variables[lastVariablePosition].array_count].variable_type = var_float_type;
            globalVariableArray.variables[lastVariablePosition].array_value[globalVariableArray.variables[lastVariablePosition].array_count].float_value = globalFunctionReturnArray.functionReturns[x].float_value;
        } else if(globalFunctionReturnArray.functionReturns[x].returnType == ret_integer_type) {
            globalVariableArray.variables[lastVariablePosition].array_value[globalVariableArray.variables[lastVariablePosition].array_count].variable_type = var_integer_type;
            globalVariableArray.variables[lastVariablePosition].array_value[globalVariableArray.variables[lastVariablePosition].array_count].integer_value = globalFunctionReturnArray.functionReturns[x].integer_value;
        } else if(globalFunctionReturnArray.functionReturns[x].returnType == ret_string_type) {
            globalVariableArray.variables[lastVariablePosition].array_value[globalVariableArray.variables[lastVariablePosition].array_count].variable_type = var_string_type;
            strcpy(globalVariableArray.variables[lastVariablePosition].array_value[globalVariableArray.variables[lastVariablePosition].array_count].string_value, globalFunctionReturnArray.functionReturns[x].string_value);
        } else {
            //none type
            globalVariableArray.variables[lastVariablePosition].array_value[globalVariableArray.variables[lastVariablePosition].array_count].variable_type = var_none_type;
        }

        globalVariableArray.variables[lastVariablePosition].array_count += 1;
    }
}

void setArrayReturn(int variablePosition) {
    for(int x=0; x < globalVariableArray.variables[variablePosition].array_count; x++) {
        
        if(globalVariableArray.variables[variablePosition].array_value[x].variable_type == var_float_type) {
            globalFunctionReturnArray.functionReturns[globalFunctionReturnArray.functionReturnCount].returnType = ret_float_type;
            globalFunctionReturnArray.functionReturns[globalFunctionReturnArray.functionReturnCount].float_value = globalVariableArray.variables[variablePosition].array_value[x].float_value;
        } else if(globalVariableArray.variables[variablePosition].array_value[x].variable_type == var_integer_type) {
            globalFunctionReturnArray.functionReturns[globalFunctionReturnArray.functionReturnCount].returnType = ret_integer_type;
            globalFunctionReturnArray.functionReturns[globalFunctionReturnArray.functionReturnCount].integer_value = globalVariableArray.variables[variablePosition].array_value[x].integer_value;
        } else if(globalVariableArray.variables[variablePosition].array_value[x].variable_type == var_string_type) {
            globalFunctionReturnArray.functionReturns[globalFunctionReturnArray.functionReturnCount].returnType = ret_string_type;
            strcpy(globalFunctionReturnArray.functionReturns[globalFunctionReturnArray.functionReturnCount].string_value, globalVariableArray.variables[variablePosition].array_value[x].string_value);
        } else {
            //none type
            globalFunctionReturnArray.functionReturns[globalFunctionReturnArray.functionReturnCount].returnType = ret_none_type;
        }

        globalFunctionReturnArray.functionReturnCount += 1;
    }  
}

void setArrayArgument(int variablePosition, Argument * argument) {

    for(int x=0; x < globalVariableArray.variables[variablePosition].array_count; x++) {

        if(globalVariableArray.variables[variablePosition].array_value[x].variable_type == var_float_type) {
            argument->array_value[argument->array_count].argumentType = arg_float_type;
            argument->array_value[argument->array_count].float_value = globalVariableArray.variables[variablePosition].array_value[x].float_value;
        } else if(globalVariableArray.variables[variablePosition].array_value[x].variable_type == var_integer_type) {
            argument->array_value[argument->array_count].argumentType = arg_integer_type;
            argument->array_value[argument->array_count].integer_value = globalVariableArray.variables[variablePosition].array_value[x].integer_value;
        } else if(globalVariableArray.variables[variablePosition].array_value[x].variable_type == var_string_type) {
            argument->array_value[argument->array_count].argumentType = arg_string_type;
            strcpy(argument->array_value[argument->array_count].string_value, globalVariableArray.variables[variablePosition].array_value[x].string_value);
        } else {
            //none type
            argument->array_value[argument->array_count].argumentType = arg_none_type;
        }

        argument->array_count += 1;
    }
}

void setArrayItem(int variablePosition, int variablePosition2) {

    for(int x=0; x < globalVariableArray.variables[variablePosition2].array_count; x++) {

        if(globalVariableArray.variables[variablePosition2].array_value[x].variable_type == var_float_type) {
            globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_float_type;
            globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].float_value = globalVariableArray.variables[variablePosition2].array_value[x].float_value;
        } else if(globalVariableArray.variables[variablePosition2].array_value[x].variable_type == var_integer_type) {
            globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_integer_type;
            globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].integer_value = globalVariableArray.variables[variablePosition2].array_value[x].integer_value;
        } else if(globalVariableArray.variables[variablePosition2].array_value[x].variable_type == var_string_type) {
            globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_string_type;
            strcpy(globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].string_value, globalVariableArray.variables[variablePosition2].array_value[x].string_value);
        } else {
            //none type
            globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_none_type;
        }

        globalVariableArray.variables[variablePosition].array_count += 1;
    }

}

int addArrayItem(Token token, int variablePosition, char scopeName[]) {
    int ret = 0;
    int isVariablesExists = F;
    int variablePosition2 = 0;

    if(token.tokenType == string_token) {
        globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_string_type;
        strcpy(globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].string_value, token.tokenValue);
    } else if(token.tokenType == integer_token) {
        globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_integer_type;
        globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].integer_value = atoi(token.tokenValue);
    } else if(token.tokenType == float_token) {
        globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_float_type;
        globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].float_value = atof(token.tokenValue);
    } else if(token.tokenType == identifier_token) {
        isVariablesExists = isVariableExists(&variablePosition2, token.tokenValue, scopeName);
        if(!isVariablesExists) {
            return unexpected_error(token.tokenLine, token.tokenColumn, "Undefined variable ", token.tokenValue, token.fileName);
        }

        switch(globalVariableArray.variables[variablePosition2].variable_type) {
            case var_float_type:
                globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_float_type;
                globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].float_value = globalVariableArray.variables[variablePosition2].float_value;
            break;
            case var_integer_type:
                globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_integer_type;
                globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].integer_value = globalVariableArray.variables[variablePosition2].integer_value;
            break;
            case var_string_type:
                globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_string_type;
                strcpy(globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].string_value ,globalVariableArray.variables[variablePosition2].string_value);
            break;
            case var_none_type:
                globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_none_type;
            break;
            default:
                return syntax_error(token.tokenLine, token.tokenColumn, "Cannot add array type as an item", token.fileName);
        }

    }

    globalVariableArray.variables[variablePosition].array_count += 1;

    return ret;
}

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
        } else if(referenceArgument.arguments[x].argumentType == arg_array_type) {
            globalVariableArray.variables[variablePosition].variable_type = var_array_type;

            globalVariableArray.variables[variablePosition].array_count = 0;
            if(!globalVariableArray.variables[variablePosition].array_init) {
                //init array
                globalVariableArray.variables[variablePosition].array_init = T;
                globalVariableArray.variables[variablePosition].array_value = malloc(TITIK_VARIABLE_INIT_LENGTH * sizeof(Variable));
            }

            for(int x2=0; x2 < referenceArgument.arguments[x].array_count; x2++) {

                if(referenceArgument.arguments[x].array_value[x2].argumentType == arg_float_type) {
                    globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_float_type;
                    globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].float_value = referenceArgument.arguments[x].array_value[x2].float_value;
                } else if(referenceArgument.arguments[x].array_value[x2].argumentType == arg_integer_type) {
                    globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_integer_type;
                    globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].integer_value = referenceArgument.arguments[x].array_value[x2].integer_value;
                } else if(referenceArgument.arguments[x].array_value[x2].argumentType == arg_string_type) {
                    globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_string_type;
                    strcpy(globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].string_value, referenceArgument.arguments[x].array_value[x2].string_value);
                } else {
                    //none type
                    globalVariableArray.variables[variablePosition].array_value[globalVariableArray.variables[variablePosition].array_count].variable_type = var_none_type;
                }

                globalVariableArray.variables[variablePosition].array_count += 1;
            }

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
    char tempChar[TITIK_CHAR_PER_LINE];
    char convertedChar[TITIK_CHAR_PER_LINE];
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
        case arg_array_type:
            //printf("Array(%d)\n", argumentArray.arguments[0].array_count);
            strcpy(tempChar, "[");
            for(int x=0; x < argumentArray.arguments[0].array_count; x++) {
                switch(argumentArray.arguments[0].array_value[x].argumentType) {
                    case arg_integer_type:
                        snprintf(convertedChar, TITIK_CHAR_PER_LINE, "%ld", argumentArray.arguments[0].array_value[x].integer_value);
                        strcat(tempChar, convertedChar);
                    break;
                    case arg_float_type:
                        snprintf(convertedChar, TITIK_CHAR_PER_LINE, "%f", argumentArray.arguments[0].array_value[x].float_value);
                        strcat(tempChar, convertedChar);
                    break;
                    case arg_string_type:
                        strcat(tempChar, "'");
                        strcat(tempChar, argumentArray.arguments[0].array_value[x].string_value);
                        strcat(tempChar, "'");
                    break;
                    default:
                        //none
                        strcat(tempChar, "Nil");
                }

                if(x+1 < argumentArray.arguments[0].array_count) {
                    strcat(tempChar, ", ");
                }
            }
            strcat(tempChar, "]");
            printf("%s\n", tempChar);
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
        case arg_array_type:
            printf("Array(%d)\n", argumentArray.arguments[0].array_count);
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
        case arg_array_type:
            *intReturn = 1;
            printf("Error: Can't convert Array type to integer\n");
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
        case arg_array_type:
            *intReturn = 1;
            printf("Error: Can't convert Array type to float\n");
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
    char tempChar2[TITIK_CHAR_PER_LINE];

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
        case arg_array_type:
            snprintf(tempChar2, TITIK_CHAR_PER_LINE, "%d", argumentArray.arguments[0].array_count);
            strcpy(tempChar, "Array(");
            strcat(tempChar, tempChar2);
            strcat(tempChar, ")");
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

//helper for flcp_execute & flmv_execute
int cpmv(char sourceFile[], char targetFile[], int removeIt) {
    FILE *source, *target;
    char ch;

    source = fopen(sourceFile, "r");

   if(source != NULL){
        target = fopen(targetFile, "w");

        if(target == NULL) {
            fclose(source);
        } else {
            while((ch = fgetc(source)) != EOF) {
                fputc(ch, target);
            }
            fclose(source);
            fclose(target);

            if(removeIt) {
                remove(sourceFile);
            }

            return T;
        }
   }

   return F;
}

void flcp_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {

    *intReturn = 0;
    funcReturn->returnType = ret_string_type;
    strcpy(funcReturn->string_value, "");

    if(argumentArray.arguments[0].argumentType != arg_string_type) {
        *intReturn = 1;
        printf("Error: Parameter must be a string\n");
    }

    if(argumentArray.arguments[1].argumentType != arg_string_type) {
        *intReturn = 1;
        printf("Error: Parameter must be a string\n");
    }

    if(cpmv(argumentArray.arguments[0].string_value, argumentArray.arguments[1].string_value, F)) {
        strcpy(funcReturn->string_value, argumentArray.arguments[1].string_value);
    }
}

void flmv_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {

    *intReturn = 0;
    funcReturn->returnType = ret_string_type;
    strcpy(funcReturn->string_value, "");

    if(argumentArray.arguments[0].argumentType != arg_string_type) {
        *intReturn = 1;
        printf("Error: Parameter must be a string\n");
    }

    if(argumentArray.arguments[1].argumentType != arg_string_type) {
        *intReturn = 1;
        printf("Error: Parameter must be a string\n");
    }

    if(cpmv(argumentArray.arguments[0].string_value, argumentArray.arguments[1].string_value, T)) {
        strcpy(funcReturn->string_value, argumentArray.arguments[1].string_value);
    }
}

void flrm_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_integer_type;
    funcReturn->integer_value = T;
    int ret;

    if(argumentArray.arguments[0].argumentType != arg_string_type) {
        *intReturn = 1;
        printf("Error: Parameter must be a string\n");
    }

    ret = remove(argumentArray.arguments[0].string_value);

    if(ret) {
        funcReturn->integer_value = F;
    }

}

void exe_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_integer_type;
    funcReturn->integer_value = T;
    int ret;

    if(argumentArray.arguments[0].argumentType != arg_string_type) {
        *intReturn = 1;
        printf("Error: Parameter must be a string\n");
    }

    ret = system(argumentArray.arguments[0].string_value);

    if(ret) {
        funcReturn->integer_value = F;
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

void rnd_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    //srand(time(NULL));
    *intReturn = 0;
    funcReturn->returnType = ret_integer_type;

    if(argumentArray.arguments[0].argumentType != arg_integer_type) {
        *intReturn = 1;
        printf("Error: Parameter must be an integer\n");
    }

    /*
    if(argumentArray.arguments[1].argumentType != arg_integer_type) {
        *intReturn = 1;
        printf("Error: Parameter must be an integer\n");
    }
    */
    /*
    int n = argumentArray.arguments[0].integer_value + rand() / (RAND_MAX / (argumentArray.arguments[1].integer_value - argumentArray.arguments[0].integer_value + 1) + 1);
    n = argumentArray.arguments[0].integer_value + rand() / (RAND_MAX / (argumentArray.arguments[1].integer_value - argumentArray.arguments[0].integer_value + 1) + 1);
    //printf("%d\n", n);
    */
    int n;

    //do {
        n = rand() % (argumentArray.arguments[0].integer_value + 1);
    //} while(n < argumentArray.arguments[0].integer_value);

    funcReturn->integer_value = n;
}

void sac_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_integer_type;
    funcReturn->integer_value = globalArgC;
}

void savf_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_string_type;
    
    strcpy(funcReturn->string_value, "");

    if(globalArgC > 2) {
        strcpy(funcReturn->string_value, globalArgV[2]);
    }
}

void sav_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_array_type;

    if(globalArgC > 2) {
        for(int x=0; x < globalArgC; x++) {
            if(x > 1) {
                strcpy(globalFunctionReturnArray.functionReturns[globalFunctionReturnArray.functionReturnCount].string_value, globalArgV[x]);
                globalFunctionReturnArray.functionReturns[globalFunctionReturnArray.functionReturnCount].returnType = ret_string_type;
                globalFunctionReturnArray.functionReturnCount += 1;
            }
        }
    } else {
        globalFunctionReturnArray.functionReturnCount = 0;
    }
}

void rndstr_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_string_type;

    if(argumentArray.arguments[0].argumentType != arg_integer_type) {
        *intReturn = 1;
        printf("Error: Parameter must be an integer\n");
    }

    rand_str(funcReturn->string_value, argumentArray.arguments[0].integer_value);
}

void senv_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_none_type;

    if(argumentArray.arguments[0].argumentType != arg_string_type) {
        *intReturn = 1;
        printf("Error: Parameter must be a string\n");
    }
    
    if(argumentArray.arguments[1].argumentType != arg_string_type) {
        *intReturn = 1;
        printf("Error: Parameter must be a string\n");
    }

    #ifdef _WIN32
        _putenv_s(argumentArray.arguments[0].string_value, argumentArray.arguments[1].string_value);
    #else
        setenv(argumentArray.arguments[0].string_value, argumentArray.arguments[1].string_value, 1);
    #endif
}

void genv_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_string_type;

    if(argumentArray.arguments[0].argumentType != arg_string_type) {
        *intReturn = 1;
        printf("Error: Parameter must be a string\n");
    }

    char *tempChar = getenv(argumentArray.arguments[0].string_value);

    if(tempChar == NULL) {
        strcpy(funcReturn->string_value, "");
    } else {
        strcpy(funcReturn->string_value, tempChar);
    }
}

void as_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_integer_type;

    if(argumentArray.arguments[0].argumentType != arg_array_type) {
        *intReturn = 1;
        printf("Error: Parameter must be an array\n");
    }

    funcReturn->integer_value = argumentArray.arguments[0].array_count;
}

#ifdef T_W_MYSQL
void mycon_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_integer_type;
    funcReturn->integer_value = F;

    for(int x=0; x < 4; x++) {
        if(argumentArray.arguments[x].argumentType != arg_string_type) {
            *intReturn = 1;
            printf("Error: Parameter must be a string\n");
        }
    }

    if(!globalMySQLIsConnected) {
        globalMySQLConnection = mysql_init(NULL);
        
        if(globalMySQLConnection != NULL) {
            if(!globalMySQLIsConnected) {
                if (mysql_real_connect(globalMySQLConnection, argumentArray.arguments[0].string_value, argumentArray.arguments[1].string_value, argumentArray.arguments[2].string_value,argumentArray.arguments[3].string_value, 0, NULL, 0) != NULL) {
                    funcReturn->integer_value = T;
                    globalMySQLIsConnected = T;
                }
            }
        }
    }
}

void myc_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_none_type;
    
    if(globalMySQLIsConnected) {
        globalMySQLIsConnected = F;
        mysql_close(globalMySQLConnection);
    }
}

void myq_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_integer_type;
    funcReturn->integer_value = F;

    if(argumentArray.arguments[0].argumentType != arg_string_type) {
        *intReturn = 1;
        printf("Error: Parameter must be a string\n");
    }

    if(globalMySQLIsConnected) {
        if(!mysql_query(globalMySQLConnection, argumentArray.arguments[0].string_value)) {
            funcReturn->integer_value = T;
        }
    }

}
void myqr_execute(ArgumentArray argumentArray, int * intReturn, FunctionReturn * funcReturn) {
    *intReturn = 0;
    funcReturn->returnType = ret_array_type;
    globalFunctionReturnArray.functionReturnCount = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;
    MYSQL_FIELD *fields;
    int strCompRet = 0;
    int fieldCtr = 0;

    for(int x=0; x < 2; x++) {
        if(argumentArray.arguments[x].argumentType != arg_string_type) {
            *intReturn = 1;
            printf("Error: Parameter must be a string\n");
        }
    }

    if(globalMySQLIsConnected) {
        if (!mysql_query(globalMySQLConnection, argumentArray.arguments[0].string_value)) {
            result = mysql_store_result(globalMySQLConnection);

            while((fields = mysql_fetch_field(result))) {

                strCompRet = strcmp(argumentArray.arguments[1].string_value, fields->name);                
                if(!(strCompRet < 0) && !(strCompRet > 0)) {
                    while ((row = mysql_fetch_row(result))) {
                        strcpy(globalFunctionReturnArray.functionReturns[globalFunctionReturnArray.functionReturnCount].string_value, row[fieldCtr]);
                        globalFunctionReturnArray.functionReturns[globalFunctionReturnArray.functionReturnCount].returnType = ret_string_type;
                        globalFunctionReturnArray.functionReturnCount += 1;
                    }
                }
                fieldCtr += 1;
            }

            mysql_free_result(result);
        }
    }
}
#endif

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

    //flmv function
    ArgumentArray flmvArgArray;
    flmvArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    flmvArgArray.argumentCount = 2;
    defineFunction("flmv", flmvArgArray, flmv_execute, T);
    //end flmv function

    //flrm function
    ArgumentArray flrmArgArray;
    flrmArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    flrmArgArray.argumentCount = 1;
    defineFunction("flrm", flrmArgArray, flrm_execute, T);
    //end flrm function

    //exe function
    ArgumentArray exeArgArray;
    exeArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    exeArgArray.argumentCount = 1;
    defineFunction("exe", exeArgArray, exe_execute, T);
    //end exe function

    //rnd function
    ArgumentArray rndArgArray;
    rndArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    rndArgArray.argumentCount = 1;
    defineFunction("rnd", rndArgArray, rnd_execute, T);
    //end rnd function

    //sac function
    ArgumentArray sacArgArray;
    sacArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    sacArgArray.argumentCount = 0;
    defineFunction("sac", sacArgArray, sac_execute, T);
    //end sac function

    //savf function
    ArgumentArray savfArgArray;
    savfArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    savfArgArray.argumentCount = 0;
    defineFunction("savf", savfArgArray, savf_execute, T);
    //end savf function

    //sav function
    ArgumentArray savArgArray;
    savArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    savArgArray.argumentCount = 0;
    defineFunction("sav", savArgArray, sav_execute, T);
    //end sav function

    //rndstr function
    ArgumentArray rndstrArgArray;
    rndstrArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    rndstrArgArray.argumentCount = 1;
    defineFunction("rndstr", rndstrArgArray, rndstr_execute, T);
    //end rndstr function

    //senv function
    ArgumentArray senvArgArray;
    senvArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    senvArgArray.argumentCount = 2;
    defineFunction("senv", senvArgArray, senv_execute, T);
    //end senv function

    //genv function
    ArgumentArray genvArgArray;
    genvArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    genvArgArray.argumentCount = 1;
    defineFunction("genv", genvArgArray, genv_execute, T);
    //end genv function

    //as function
    ArgumentArray asArgArray;
    asArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    asArgArray.argumentCount = 1;
    defineFunction("as", asArgArray, as_execute, T);
    //end as function

    #ifdef T_W_MYSQL
    //mycon function
    ArgumentArray myconArgArray;
    myconArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    myconArgArray.argumentCount = 4;
    defineFunction("mycon", myconArgArray, mycon_execute, T);
    //end mycon function

    //myc function
    ArgumentArray mycArgArray;
    mycArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    mycArgArray.argumentCount = 0;
    defineFunction("myc", mycArgArray, myc_execute, T);
    //end myc function

    //myq function
    ArgumentArray myqArgArray;
    myqArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    myqArgArray.argumentCount = 1;
    defineFunction("myq", myqArgArray, myq_execute, T);
    //end myq function
    
    //myqr function
    ArgumentArray myqrArgArray;
    myqrArgArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    myqrArgArray.argumentCount = 2;
    defineFunction("myqr", myqrArgArray, myqr_execute, T);
    //end myqr function
    #endif
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