/* parser.c
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#include "parser.h"
#include "lexer.h"
#include "debug.h"
#include "util.h"
#include "functions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

extern FunctionArray globalFunctionArray;
extern VariableArray globalVariableArray;

TokenArray stripUnwantedToken(TokenArray tokenArray) {
    TokenArray newTokens;
    newTokens.tokens = malloc(TITIK_TOKEN_INIT_LENGTH * sizeof(Token));
    newTokens.tokenCount = 0;

    for(int x=0; x < tokenArray.tokenCount; x++) {
        //remove space, newline, comments
        if(tokenArray.tokens[x].tokenType == newline_token || tokenArray.tokens[x].tokenType == space_token || tokenArray.tokens[x].tokenType == single_comment_token || tokenArray.tokens[x].tokenType == multi_comment_token || tokenArray.tokens[x].tokenType == close_multi_comment_token || tokenArray.tokens[x].tokenType == close_string_token) {
            continue;
        } else {
            newTokens.tokens[newTokens.tokenCount].tokenLine = tokenArray.tokens[x].tokenLine;
            newTokens.tokens[newTokens.tokenCount].tokenColumn = tokenArray.tokens[x].tokenColumn;
            newTokens.tokens[newTokens.tokenCount].tokenType = tokenArray.tokens[x].tokenType;
            strcpy(newTokens.tokens[newTokens.tokenCount].tokenValue, tokenArray.tokens[x].tokenValue);
            strcpy(newTokens.tokens[newTokens.tokenCount].fileName, tokenArray.tokens[x].fileName);
            newTokens.tokenCount += 1;
        }
    }

    return newTokens;
}

int parseToken(TokenArray tokenArray) {
    ParserState parserState = get_start;
    TokenArray strippedToken;
    FunctionReturn funcReturn;
    int isFunctionsExists = F;
    int functionPosition = 0;
    int isVariablesExists = F;
    int variablePosition = 0;
    int variablePosition2 = 0;
    int intFunctionReturn = 0;
    Token currentIdentifier;
    TokenType currentOperation = none_token;
    char tempChar[TITIK_VARIABLE_INIT_LENGTH];
    //strip all spaces and newline first before parsing the token
    strippedToken = stripUnwantedToken(tokenArray);

    ArgumentArray argumentArray;
    argumentArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    argumentArray.argumentCount = 0;

    //for debugging purpose only
    //dumpToken(strippedToken);
    //end for debugging purpose only

    for(int x=0; x < strippedToken.tokenCount; x++) {

        switch(parserState) {
            case get_start:

                switch(strippedToken.tokens[x].tokenType) {
                    case identifier_token:
                        parserState = get_assignment_or_function;
                        //reset values
                        isVariablesExists = F;
                        variablePosition = 0;
                        //set current identifier
                        currentIdentifier.tokenType = identifier_token;
                        strcpy(currentIdentifier.tokenValue, strippedToken.tokens[x].tokenValue);
                        currentIdentifier.tokenLine = strippedToken.tokens[x].tokenLine;
                        currentIdentifier.tokenColumn = strippedToken.tokens[x].tokenColumn;
                        strcpy(currentIdentifier.fileName, strippedToken.tokens[x].fileName);
                    break;
                    case keyword_token:
                        //check first if in functions array
                        //if then check for arguments (check if matched)
                        //then execute the function
                        isFunctionsExists = F;
                        functionPosition = 0;

                        isFunctionsExists = isFunctionExists(&functionPosition, strippedToken.tokens[x].tokenValue);

                        if(isFunctionsExists) {
                            argumentArray.argumentCount = 0;
                            parserState = get_function_open_parenthesis;
                        } else {
                            return unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected keyword ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);                            
                        }
                    break;
                    default:
                        return unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                }

            break;
            case get_function_open_parenthesis:
                if(strippedToken.tokens[x].tokenType == open_parenthesis_token) {
                    parserState = get_function_parameters;
                } else {
                    return syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Expecting '('", strippedToken.tokens[x].fileName);
                }
            break;
            case get_function_parameters:
                if(strippedToken.tokens[x].tokenType == close_parenthesis_token) {
                    parserState = get_start;
                    
                    //check first if # of arguments matched
                    if(argumentArray.argumentCount != globalFunctionArray.functions[functionPosition].argumentArray.argumentCount) {
                        return syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Argument count didn't matched", strippedToken.tokens[x].fileName);
                    }

                    //execute function
                    globalFunctionArray.functions[functionPosition].execute(argumentArray, &intFunctionReturn, &funcReturn);
                    //set return value & type
                    globalFunctionArray.functions[functionPosition].functionReturn = funcReturn;

                    if(intFunctionReturn > 0) {
                        return intFunctionReturn;
                    }

                } else if(strippedToken.tokens[x].tokenType == string_token || strippedToken.tokens[x].tokenType == float_token || strippedToken.tokens[x].tokenType == integer_token || strippedToken.tokens[x].tokenType == identifier_token) {
                    //add to arguments

                    if(strippedToken.tokens[x].tokenType == identifier_token) {
                        isVariablesExists = F;
                        variablePosition = 0;

                        isVariablesExists = isVariableExists(&variablePosition, strippedToken.tokens[x].tokenValue, TITIK_MAIN_SCOPE_NAME);
                        if(!isVariablesExists) {
                            return unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Undefined variable ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        }
                    }

                    if(strippedToken.tokens[x].tokenType == string_token) {
                        argumentArray.arguments[argumentArray.argumentCount].argumentType = arg_string_type;
                        strcpy(argumentArray.arguments[argumentArray.argumentCount].string_value, strippedToken.tokens[x].tokenValue);
                    } else if(strippedToken.tokens[x].tokenType == float_token) {
                        argumentArray.arguments[argumentArray.argumentCount].argumentType = arg_float_type;
                        argumentArray.arguments[argumentArray.argumentCount].float_value = atof(strippedToken.tokens[x].tokenValue);
                    } else if(strippedToken.tokens[x].tokenType == integer_token) {
                        argumentArray.arguments[argumentArray.argumentCount].argumentType = arg_integer_type;
                        argumentArray.arguments[argumentArray.argumentCount].integer_value = atoi(strippedToken.tokens[x].tokenValue);
                    } else if(strippedToken.tokens[x].tokenType == identifier_token) {

                        if(globalVariableArray.variables[variablePosition].variable_type == var_string_type) {
                            argumentArray.arguments[argumentArray.argumentCount].argumentType = arg_string_type;
                            strcpy(argumentArray.arguments[argumentArray.argumentCount].string_value, globalVariableArray.variables[variablePosition].string_value);                           
                        } else if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                            argumentArray.arguments[argumentArray.argumentCount].argumentType = arg_integer_type;
                            argumentArray.arguments[argumentArray.argumentCount].integer_value = globalVariableArray.variables[variablePosition].integer_value;                           
                        } else if(globalVariableArray.variables[variablePosition].variable_type == var_float_type) {
                            argumentArray.arguments[argumentArray.argumentCount].argumentType = arg_float_type;
                            argumentArray.arguments[argumentArray.argumentCount].float_value = globalVariableArray.variables[variablePosition].float_value;                           
                        } else if(globalVariableArray.variables[variablePosition].variable_type == var_none_type) {
                            argumentArray.arguments[argumentArray.argumentCount].argumentType = arg_none_type;                        
                        }

                    }

                    argumentArray.argumentCount += 1;
                } else if(strippedToken.tokens[x].tokenType == comma_token) {
                    if(strippedToken.tokens[x - 1].tokenType == string_token || strippedToken.tokens[x - 1].tokenType == float_token || strippedToken.tokens[x - 1].tokenType == integer_token) {
                        //then ok
                        //nothing to do
                    } else {
                        return syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ','", strippedToken.tokens[x].fileName);
                    }
                } else {
                    return unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                }
            break;
            case get_assignment_or_function:
                //check if executing function or assigment statement
                //add else if here to check execution of function
                if(strippedToken.tokens[x].tokenType == equals_token) {
                    //assignment 
                    parserState = get_assignment_value;
                } else {
                    //unexpected token
                    return unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                }
            break;
            case get_assignment_value:
                //reset current operation
                currentOperation = none_token;

                //check variable if existing and set the variable value here
                if(strippedToken.tokens[x].tokenType == string_token || strippedToken.tokens[x].tokenType == float_token || strippedToken.tokens[x].tokenType == integer_token || strippedToken.tokens[x].tokenType == identifier_token) {

                    isVariablesExists = isVariableExists(&variablePosition, currentIdentifier.tokenValue, TITIK_MAIN_SCOPE_NAME);
                    
                    if(!isVariablesExists) {
                        variablePosition = globalVariableArray.variableCount;
                        globalVariableArray.variableCount += 1;
                    } else {
                        if(globalVariableArray.variables[variablePosition].is_constant) {
                            return unexpected_error(currentIdentifier.tokenLine, currentIdentifier.tokenColumn, "Cannot override constant ", currentIdentifier.tokenValue, currentIdentifier.fileName);
                        }
                    }

                    if(strippedToken.tokens[x].tokenType == identifier_token) {
                        isVariablesExists = F;
                        variablePosition2 = 0;

                        isVariablesExists = isVariableExists(&variablePosition2, strippedToken.tokens[x].tokenValue, TITIK_MAIN_SCOPE_NAME);
                        if(!isVariablesExists) {
                            return unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Undefined variable ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        }
                    }

                    //set variable type and value
                    if(strippedToken.tokens[x].tokenType == string_token) {
                        globalVariableArray.variables[variablePosition].variable_type = var_string_type;
                        strcpy(globalVariableArray.variables[variablePosition].string_value, strippedToken.tokens[x].tokenValue);
                    } else if(strippedToken.tokens[x].tokenType == float_token) {
                        globalVariableArray.variables[variablePosition].variable_type = var_float_type;
                        globalVariableArray.variables[variablePosition].float_value = atof(strippedToken.tokens[x].tokenValue);
                    } else if(strippedToken.tokens[x].tokenType == integer_token) {
                        globalVariableArray.variables[variablePosition].integer_value = atoi(strippedToken.tokens[x].tokenValue);
                        globalVariableArray.variables[variablePosition].variable_type = var_integer_type;
                    } else if(strippedToken.tokens[x].tokenType == identifier_token) {

                        if(globalVariableArray.variables[variablePosition2].variable_type == var_string_type) {
                            globalVariableArray.variables[variablePosition].variable_type = var_string_type;
                            strcpy(globalVariableArray.variables[variablePosition].string_value, globalVariableArray.variables[variablePosition2].string_value);
                        } else if(globalVariableArray.variables[variablePosition2].variable_type == var_integer_type) {
                            globalVariableArray.variables[variablePosition].integer_value = globalVariableArray.variables[variablePosition2].integer_value;
                            globalVariableArray.variables[variablePosition].variable_type = var_integer_type;     
                        } else if(globalVariableArray.variables[variablePosition2].variable_type == var_float_type) {
                            globalVariableArray.variables[variablePosition].variable_type = var_float_type;
                            globalVariableArray.variables[variablePosition].float_value = globalVariableArray.variables[variablePosition2].float_value;                
                        } else if(globalVariableArray.variables[variablePosition2].variable_type == var_none_type) {
                            globalVariableArray.variables[variablePosition].variable_type = var_none_type;                
                        }

                    }

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

                    //Check and advance the token array if the next statement
                    // is a concatenation or any other operation
                    //if it is then stay to get_assigment_value parserState
                    //else set to get_start

                    if((x+1) < strippedToken.tokenCount) {
                        if(strippedToken.tokens[x+1].tokenType == plus_token || strippedToken.tokens[x+1].tokenType == minus_token || strippedToken.tokens[x+1].tokenType == divide_token || strippedToken.tokens[x+1].tokenType == multiply_token) {
                            //will perform operation
                            parserState = get_assigment_operation;
                        } else {
                            parserState = get_start;
                        }
                    } else {
                        parserState = get_start;
                    }
                    
                } else {
                    return unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                }
            break;
            case get_assigment_operation:
                //get operation
                currentOperation = strippedToken.tokens[x].tokenType;
                parserState = update_assigment_value;

                if(!((x+1) < strippedToken.tokenCount)) {
                    return syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unfinished operation", strippedToken.tokens[x].fileName);
                }
            break;
            case update_assigment_value:
                //update variable depending on the operator
                if(strippedToken.tokens[x].tokenType == string_token || strippedToken.tokens[x].tokenType == float_token || strippedToken.tokens[x].tokenType == integer_token || strippedToken.tokens[x].tokenType == identifier_token) {

                    //TODO: handle identifier_token
                    //update
                    switch(globalVariableArray.variables[variablePosition].variable_type) {
                        case var_float_type:
                        break;
                        case var_integer_type:
                        break;
                        case var_string_type:
                            //concatenate only

                            if(currentOperation == plus_token) {
                                if(strippedToken.tokens[x].tokenType == identifier_token) {
                                    //check if existing variable
                                    isVariablesExists = F;
                                    variablePosition2 = 0;
                                    strcpy(tempChar, ""); //clear temp char

                                    isVariablesExists = isVariableExists(&variablePosition2, strippedToken.tokens[x].tokenValue, TITIK_MAIN_SCOPE_NAME);
                                    if(!isVariablesExists) {
                                        return unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Undefined variable ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                    }

                                    switch(globalVariableArray.variables[variablePosition2].variable_type) {
                                        case var_float_type:
                                            snprintf(tempChar, TITIK_VARIABLE_INIT_LENGTH, "%f", globalVariableArray.variables[variablePosition2].float_value);
                                            strcat(globalVariableArray.variables[variablePosition].string_value, tempChar);
                                        break;
                                        case var_string_type:
                                            strcat(globalVariableArray.variables[variablePosition].string_value, globalVariableArray.variables[variablePosition2].string_value);
                                        break;
                                        case var_integer_type:
                                            snprintf(tempChar, TITIK_VARIABLE_INIT_LENGTH, "%ld", globalVariableArray.variables[variablePosition2].integer_value);
                                            strcat(globalVariableArray.variables[variablePosition].string_value, tempChar);
                                        break;
                                        default:
                                            //variable is a none type
                                            return syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "You cannot perform an operation with a None type", strippedToken.tokens[x].fileName);
                                    }

                                } else {
                                    strcat(globalVariableArray.variables[variablePosition].string_value, strippedToken.tokens[x].tokenValue);
                                }

                            } else {
                                return unexpected_error(strippedToken.tokens[x-1].tokenLine, strippedToken.tokens[x-1].tokenColumn, "Invalid operation ", strippedToken.tokens[x-1].tokenValue, strippedToken.tokens[x-1].fileName);
                            }

                        break;
                        default:
                            //none type
                            return syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "You cannot perform an operation with a None type", strippedToken.tokens[x].fileName);
                    }

                    parserState = get_start; //TODO: <--- remove that!! TEMPORARY ONLY (ONLY ACCEPTS 1 OPERAND) --- FOR TESTING ONLY
                } else {
                    return unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                }
            break;
            default:
                continue;
        }

    }

    return 0;
}