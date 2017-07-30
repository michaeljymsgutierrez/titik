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

void checkOperationAndSetParser(int x, ParserState * parserState, TokenArray strippedToken) {
    if((x+1) < strippedToken.tokenCount) {
        if(strippedToken.tokens[x+1].tokenType == plus_token || strippedToken.tokens[x+1].tokenType == minus_token || strippedToken.tokens[x+1].tokenType == divide_token || strippedToken.tokens[x+1].tokenType == multiply_token) {
            //will perform operation
            *parserState = get_assigment_operation;
        } else {
            *parserState = get_start;
        }
    } else {
        *parserState = get_start;
    }
}

void freeArrays(TokenArray * newTempTokens, ArgumentArray * argumentArray, TokenArray * newTokens) {
    free(newTempTokens->tokens);
    free(newTokens->tokens);
    free(argumentArray->arguments);
}

int parseToken(TokenArray tokenArray, int isLoop, int stripIt, int * needBreak, char currentScope[], FunctionReturn * thisReturn, int * gotReturn) {
    *gotReturn = F;
    ParserState parserState = get_start;
    TokenArray strippedToken;
    TokenArray newTokens;
    newTokens.tokens = malloc(TITIK_TOKEN_INIT_LENGTH * sizeof(Token));
    newTokens.tokenCount = 0;
    FunctionReturn funcReturn;
    thisReturn->returnType = ret_none_type; //return on function call
    int isFunctionsExists = F;
    int functionPosition = 0;
    int isArgumentExists = F;
    int argumentPosition = 0;
    int isVariablesExists = F;
    int variablePosition = 0;
    int variablePosition2 = 0;
    int intFunctionReturn = 0;
    int isParsing = T;
    int checkVar = T;
    Token currentIdentifier;
    Token currentIdentifier2;
    Variable tempVariable; //this will hold data in if comparison
    Variable tempVariable2; //this will hold data in if comparision
    TitikTokenType currentOperation = none_token;
    char tempChar[TITIK_CHAR_PER_LINE];
    int ifWithTrue = F;
    int elseIfWithTrue = F;
    int ifEndCount = 0;
    int elseIfMode = F;
    int elseIfGotStatements = F;
    int isFunctionAssignmentMode = F;
    int isFunctionAssignmentUpdate = F;
    int lastVariablePosition = 0;

    int fromLoop = 0;
    int toLoop = 0;
    int forLoopEndCount = 0;
    int whileLoopEndCount = 0;
    int functionEndCount = 0;

    int willBreak = F;
    int loopReturn = F;

    TokenArray newTempTokens;
    newTempTokens.tokens = malloc(TITIK_TOKEN_INIT_LENGTH * sizeof(Token));
    newTempTokens.tokenCount = 0;

    if(!stripIt) {
        strippedToken = tokenArray;
    } else {
        //strip all spaces and newline first before parsing the token
        for(int x=0; x < tokenArray.tokenCount; x++) {
            //remove space, newline, comments, carriage return & tab
            if(tokenArray.tokens[x].tokenType == newline_token || tokenArray.tokens[x].tokenType == space_token || tokenArray.tokens[x].tokenType == single_comment_token || tokenArray.tokens[x].tokenType == multi_comment_token || tokenArray.tokens[x].tokenType == close_multi_comment_token || tokenArray.tokens[x].tokenType == close_string_token || tokenArray.tokens[x].tokenType == tab_token || tokenArray.tokens[x].tokenType == carriage_return_token) {
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

        strippedToken = newTokens;
    }

    ArgumentArray argumentArray;
    argumentArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
    argumentArray.argumentCount = 0;

    //for debugging purpose only
    //dumpToken(strippedToken);
    //end for debugging purpose only

    while(isParsing) {
        isParsing = F;
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
                            setTemporaryToken(&currentIdentifier, strippedToken, x, identifier_token);
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

                                if(!strcmp(strippedToken.tokens[x].tokenValue, "i")) {
                                    if(strcmp(currentScope, TITIK_MAIN_SCOPE_NAME)) {
                                        intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "You cannot include file inside a function", strippedToken.tokens[x].fileName);
                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                        return intFunctionReturn;
                                    }
                                }
                            } else {
                                if(!strcmp(strippedToken.tokens[x].tokenValue, "if")) {
                                    parserState = get_if_opening;
                                    ifWithTrue = F;
                                    elseIfWithTrue = F;
                                    ifEndCount = 0;
                                    newTempTokens.tokenCount = 0;
                                    currentOperation = none_token;
                                    elseIfMode = F;
                                    elseIfGotStatements = F;
                                } else if(!strcmp(strippedToken.tokens[x].tokenValue, "fl")) {
                                    parserState = get_for_loop_opening;
                                    fromLoop = 0;
                                    toLoop = 0;
                                    forLoopEndCount = 0;
                                    willBreak = F;
                                    newTempTokens.tokenCount = 0; //if error occurs then need to have a new temporary container of the token array
                                } else if(!strcmp(strippedToken.tokens[x].tokenValue, "wl")) {
                                    parserState = get_while_loop_opening;
                                    whileLoopEndCount = 0;
                                    willBreak = F;
                                    currentOperation = none_token;
                                    newTempTokens.tokenCount = 0; 
                                } else if(!strcmp(strippedToken.tokens[x].tokenValue, "b")) {
                                    if(isLoop) {
                                        *needBreak = T;
                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                        return 0;
                                    } else {
                                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected keyword ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);   
                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                        return intFunctionReturn; 
                                    }
                                } else if(!strcmp(strippedToken.tokens[x].tokenValue, "fd")) {
                                    //function definition
                                    parserState = get_function_name;
                                    functionEndCount = 0;
                                    //prohibit declaration of function inside a function
                                    if(strcmp(currentScope, TITIK_MAIN_SCOPE_NAME)) {
                                        intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "You cannot define a function inside a function", strippedToken.tokens[x].fileName);
                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                        return intFunctionReturn;
                                    }
                                } else if(!strcmp(strippedToken.tokens[x].tokenValue, "rtn")) {
                                    if(strcmp(currentScope, TITIK_MAIN_SCOPE_NAME)) {
                                        parserState = get_return_value;
                                    } else {
                                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                        return intFunctionReturn;
                                    }
                                } else {
                                    intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected keyword ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                    freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                    return intFunctionReturn;
                                }                            
                            }
                        break;
                        default:
                            intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                    }

                break;
                case get_while_loop_opening:
                    if(strippedToken.tokens[x].tokenType == open_parenthesis_token) {
                        parserState = get_while_condition1;
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }  
                break;
                case get_while_condition1:
                    if(strippedToken.tokens[x].tokenType == string_token || strippedToken.tokens[x].tokenType == float_token || strippedToken.tokens[x].tokenType == integer_token || strippedToken.tokens[x].tokenType == identifier_token) {
                        setTemporaryToken(&currentIdentifier, strippedToken, x, strippedToken.tokens[x].tokenType);
                        parserState = get_while_operator_or_end;
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_while_operator_or_end:
                break;
                case get_return_value:
                    if(strippedToken.tokens[x].tokenType == string_token || strippedToken.tokens[x].tokenType == float_token || strippedToken.tokens[x].tokenType == integer_token || strippedToken.tokens[x].tokenType == identifier_token) {
                        switch(strippedToken.tokens[x].tokenType) {
                            case string_token:
                                thisReturn->returnType = ret_string_type;
                                strcpy(thisReturn->string_value, strippedToken.tokens[x].tokenValue);
                            break;
                            case float_token:
                                thisReturn->returnType = ret_float_type;
                                thisReturn->float_value = atof(strippedToken.tokens[x].tokenValue);
                            break;
                            case integer_token:
                                thisReturn->returnType = ret_integer_type;
                                thisReturn->integer_value = atoi(strippedToken.tokens[x].tokenValue);
                            break;
                            case identifier_token:
                                isVariablesExists = F;
                                variablePosition = 0;

                                isVariablesExists = isVariableExists(&variablePosition, strippedToken.tokens[x].tokenValue, currentScope);
                                if(!isVariablesExists) {
                                    intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Undefined variable ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                    freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                    return intFunctionReturn;
                                }

                                switch(globalVariableArray.variables[variablePosition].variable_type) {
                                    case var_float_type:
                                        thisReturn->returnType = ret_float_type;
                                        thisReturn->float_value = globalVariableArray.variables[variablePosition].float_value;
                                    break;
                                    case var_integer_type:
                                        thisReturn->returnType = ret_integer_type;
                                        thisReturn->integer_value = globalVariableArray.variables[variablePosition].integer_value;
                                    break;
                                    case var_string_type:
                                        thisReturn->returnType = ret_string_type;
                                        strcpy(thisReturn->string_value, globalVariableArray.variables[variablePosition].string_value);
                                    break;
                                    default:
                                        thisReturn->returnType = ret_none_type;
                                }
                            break;
                            default:
                                intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                return intFunctionReturn;
                        }
                        *gotReturn = T;
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return 0;
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_function_name:
                    if(strippedToken.tokens[x].tokenType == identifier_token) {
                        //check if the identifier is already defined as variable
                        //if it is then raise an error
                        isVariablesExists = F;
                        variablePosition = 0;

                        isVariablesExists = isVariableExists(&variablePosition, strippedToken.tokens[x].tokenValue, currentScope);
                        if(isVariablesExists) {
                            intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Identifier is already defined as variable", strippedToken.tokens[x].fileName);
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }

                        //check if the identifier is already defined as function
                        //if it is then raise an error that tells the user it's already existing
                        //(You cannot redefine function in Titik)
                        isFunctionsExists = F;
                        functionPosition = 0;

                        isFunctionsExists = isFunctionExists(&functionPosition, strippedToken.tokens[x].tokenValue);
                        if(isFunctionsExists) {
                            intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Function already exists", strippedToken.tokens[x].fileName);
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }

                        setTemporaryToken(&currentIdentifier, strippedToken, x, identifier_token);

                        //init function arguments
                        globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.argumentCount = 0;
                        globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.arguments = malloc(TITIK_ARGUMENT_INIT_LENGTH * sizeof(Argument));
                        //init function tokens
                        globalFunctionArray.functions[globalFunctionArray.functionCount].tokens.tokens = malloc(TITIK_TOKEN_INIT_LENGTH * sizeof(Token));
                        globalFunctionArray.functions[globalFunctionArray.functionCount].tokens.tokenCount = 0;
                        //init other function info
                        globalFunctionArray.functions[globalFunctionArray.functionCount].isSystem = F;
                        globalFunctionArray.functions[globalFunctionArray.functionCount].functionReturn.returnType = ret_none_type; //set the default return to none type

                        parserState = get_function_declaration_open_parenthesis;
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_function_declaration_open_parenthesis:
                    if(strippedToken.tokens[x].tokenType == open_parenthesis_token) {
                        parserState = get_function_declaration_parameters;
                    } else {
                        intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Expecting '('", strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_function_declaration_parameters:
                    if(strippedToken.tokens[x].tokenType == close_parenthesis_token) {
                        parserState = get_function_body; //next: get the function contents
                    } else if(strippedToken.tokens[x].tokenType == identifier_token) {
                        //add to function arguments

                        //check if the argument is already defined as a function
                        isFunctionsExists = F;
                        functionPosition = 0;

                        isFunctionsExists = isFunctionExists(&functionPosition, strippedToken.tokens[x].tokenValue);
                        if(isFunctionsExists) {
                            intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Function argument is already defined as function", strippedToken.tokens[x].fileName);
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }

                        //check if token is a constant
                        //if yes then raise an error
                        if(isupper(strippedToken.tokens[x].tokenValue[0])){
                            intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Function argument cannot be a constant", strippedToken.tokens[x].fileName);
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }

                        //check if argument is duplicate below
                        //globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.arguments
                        isArgumentExists = F;
                        argumentPosition = 0;

                        isArgumentExists = isFunctionArgumentExists(globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray, &argumentPosition, strippedToken.tokens[x].tokenValue);
                        if(isArgumentExists) {
                            intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Duplicate argument ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }

                        //set the argument below
                        globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.arguments[globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.argumentCount].argumentType = arg_none_type;
                        strcpy(globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.arguments[globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.argumentCount].argumentName, strippedToken.tokens[x].tokenValue);
                        //increment arg
                        globalFunctionArray.functions[globalFunctionArray.functionCount].argumentArray.argumentCount += 1;
                    } else if(strippedToken.tokens[x].tokenType == comma_token) {
                        if(strippedToken.tokens[x - 1].tokenType == identifier_token) {
                            //then ok
                            //nothing to do
                        } else {
                            intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ','", strippedToken.tokens[x].fileName);
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_function_body:
                    if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "df") && functionEndCount == 0) {
                        //define function
                        strcpy(globalFunctionArray.functions[globalFunctionArray.functionCount].functionName, currentIdentifier.tokenValue);
                        //increment function count here
                        globalFunctionArray.functionCount += 1;
                        
                        parserState = get_start;
                    } else {
                        if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "fd")) {
                            functionEndCount += 1;
                        }

                        if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "df")) {
                            functionEndCount -= 1;
                        }

                        //update function tokens below
                        updateTemporaryTokens(&globalFunctionArray.functions[globalFunctionArray.functionCount].tokens, strippedToken, x);
                    }
                break;
                case get_for_loop_opening:
                    if(strippedToken.tokens[x].tokenType == open_parenthesis_token) {
                        parserState = get_for_loop_from;
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }                    
                break;
                case get_for_loop_from:
                    if(strippedToken.tokens[x].tokenType == integer_token || strippedToken.tokens[x].tokenType == identifier_token) {
                        parserState = get_for_loop_to_word;
                        if(strippedToken.tokens[x].tokenType == identifier_token) {
                            isVariablesExists = F;
                            variablePosition = 0;

                            isVariablesExists = isVariableExists(&variablePosition, strippedToken.tokens[x].tokenValue, currentScope);
                            if(!isVariablesExists) {
                                intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Undefined variable ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                return intFunctionReturn;
                            }

                            if(globalVariableArray.variables[variablePosition].variable_type != var_integer_type) {
                                intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Expecting integer variable", strippedToken.tokens[x].fileName);
                                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                return intFunctionReturn;
                            }

                            fromLoop = globalVariableArray.variables[variablePosition].integer_value;
                        } else {
                            fromLoop = atoi(strippedToken.tokens[x].tokenValue);
                        }
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }  
                break;
                case get_for_loop_to_word:
                    if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "to")) {
                        parserState = get_for_loop_to;
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_for_loop_to:
                    if(strippedToken.tokens[x].tokenType == integer_token || strippedToken.tokens[x].tokenType == identifier_token) {
                        parserState = get_for_loop_end;
                        if(strippedToken.tokens[x].tokenType == identifier_token) {
                            isVariablesExists = F;
                            variablePosition = 0;

                            isVariablesExists = isVariableExists(&variablePosition, strippedToken.tokens[x].tokenValue, currentScope);
                            if(!isVariablesExists) {
                                intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Undefined variable ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                return intFunctionReturn;
                            }

                            if(globalVariableArray.variables[variablePosition].variable_type != var_integer_type) {
                                intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Expecting integer variable", strippedToken.tokens[x].fileName);
                                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                return intFunctionReturn;
                            }

                            toLoop = globalVariableArray.variables[variablePosition].integer_value;
                        } else {
                            toLoop = atoi(strippedToken.tokens[x].tokenValue);
                        }
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    } 
                break;
                case get_for_loop_end:
                    if(strippedToken.tokens[x].tokenType == close_parenthesis_token) {
                        parserState = get_for_loop_statements;
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }  
                break;
                case get_for_loop_statements:
                    if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "lf") && forLoopEndCount == 0) {
                        //execute looping statement
                        if(fromLoop < toLoop) {
                            //forward looping
                            for(int lc=fromLoop; lc <= toLoop; lc++) {
                                intFunctionReturn = parseToken(newTempTokens, T, F, &willBreak, currentScope, &funcReturn, &loopReturn);

                                if(willBreak) {
                                    break;
                                }

                                if(loopReturn) {
                                    *gotReturn = T;
                                    break;
                                }

                                if(intFunctionReturn > 0) {
                                    freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                    return intFunctionReturn;
                                    break;
                                }
                            }
                        } else if(fromLoop > toLoop) {
                            //backward looping
                            for(int lc=fromLoop; lc >= toLoop; lc--) {
                                intFunctionReturn = parseToken(newTempTokens, T, F, &willBreak, currentScope, &funcReturn, &loopReturn);

                                if(willBreak) {
                                    break;
                                }

                                if(loopReturn) {
                                    *gotReturn = T;
                                    break;
                                }

                                if(intFunctionReturn > 0) {
                                    freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                    return intFunctionReturn;
                                    break;
                                }
                            }
                        } else {
                            //execute once
                            intFunctionReturn = parseToken(newTempTokens, T, F, &willBreak, currentScope, &funcReturn, &loopReturn);
                            /*
                            if(willBreak) {
                                break;
                            }
                            */
                            if(loopReturn) {
                                *gotReturn = T;
                            }
                            if(intFunctionReturn > 0) {
                                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                return intFunctionReturn;
                                //break;
                            }
                        }

                        *thisReturn = funcReturn;

                        parserState = get_start;
                    } else {
                        if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "fl")) {
                            forLoopEndCount += 1;
                        }

                        if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "lf")) {
                            forLoopEndCount -= 1;
                        }

                        updateTemporaryTokens(&newTempTokens, strippedToken, x);
                    }
                break;
                case get_if_opening:
                    if(strippedToken.tokens[x].tokenType == open_parenthesis_token) {
                        parserState = get_if_expression1;
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_if_expression1:
                    if(strippedToken.tokens[x].tokenType == string_token || strippedToken.tokens[x].tokenType == float_token || strippedToken.tokens[x].tokenType == integer_token || strippedToken.tokens[x].tokenType == identifier_token) {
                        setTemporaryToken(&currentIdentifier, strippedToken, x, strippedToken.tokens[x].tokenType);
                        parserState = get_if_operator_or_end;
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_if_expression2:
                    if(strippedToken.tokens[x].tokenType == string_token || strippedToken.tokens[x].tokenType == float_token || strippedToken.tokens[x].tokenType == integer_token || strippedToken.tokens[x].tokenType == identifier_token) {
                        setTemporaryToken(&currentIdentifier2, strippedToken, x, strippedToken.tokens[x].tokenType);
                        parserState = get_if_end;
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_if_end:
                    if(strippedToken.tokens[x].tokenType == close_parenthesis_token) {
                        intFunctionReturn = convertTokenToVariable(&tempVariable, currentIdentifier, currentScope);

                        if(intFunctionReturn > 0) {
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }

                        intFunctionReturn = convertTokenToVariable(&tempVariable2, currentIdentifier2, currentScope);

                        if(intFunctionReturn > 0) {
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }

                        if(!elseIfMode) {
                            compareVariable(tempVariable, tempVariable2, currentOperation, &ifWithTrue);
                            parserState = get_if_statements;
                        } else {
                            
                            if(!ifWithTrue && !elseIfWithTrue) {
                                compareVariable(tempVariable, tempVariable2, currentOperation, &elseIfWithTrue);
                            }

                            parserState = get_else_if_statements;
                        }

                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_if_operator_or_end:
                    //get conditional operator or close parenthesis to end the expression
                    if(strippedToken.tokens[x].tokenType == greater_than_token || strippedToken.tokens[x].tokenType == less_than_token || strippedToken.tokens[x].tokenType == equals_token || strippedToken.tokens[x].tokenType == close_parenthesis_token) {
                        //if close parenthesis then evaluate the expression right away
                        if(strippedToken.tokens[x].tokenType == greater_than_token || strippedToken.tokens[x].tokenType == less_than_token || strippedToken.tokens[x].tokenType == equals_token) {
                            currentOperation = strippedToken.tokens[x].tokenType;
                            parserState = get_if_expression2;
                        } else {
                            //close parenthesis
                            //evaluate the expression
                            //if true then get all tokens
                            if(!elseIfMode) {
                                intFunctionReturn = evaluateToken(currentIdentifier, &ifWithTrue, currentScope);

                                if(intFunctionReturn > 0) {
                                    freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                    return intFunctionReturn;
                                }

                                parserState = get_if_statements;
                            } else {

                                if(!ifWithTrue && !elseIfWithTrue) {
                                    intFunctionReturn = evaluateToken(currentIdentifier, &elseIfWithTrue, currentScope);

                                    if(intFunctionReturn > 0) {
                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                        return intFunctionReturn;
                                    }
                                }

                                parserState = get_else_if_statements;
                            }
                        }
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case rebuild_tokens:
                    updateTemporaryTokens(&newTempTokens, strippedToken, x);

                    if((x+1) == strippedToken.tokenCount) {
                        parserState = get_start;
                        strippedToken = newTempTokens;
                    }

                break;
                case get_if_statements_else:
                    if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "fi") && ifEndCount == 0) {
                        //add the remaining unparsed token to the new token
                        isParsing = T;
                        if((x+1) < strippedToken.tokenCount) {
                            //rebuild and reparse the token
                            parserState = rebuild_tokens;
                        } else {
                            parserState = get_start;
                            strippedToken = newTempTokens;
                        }
                    } else {
                        if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "if")) {
                            //track that it's not yet the end of the current if statement
                            ifEndCount += 1;
                        }

                        if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "fi")) {
                            ifEndCount -= 1;
                        }

                        if(!ifWithTrue && !elseIfWithTrue) {
                            updateTemporaryTokens(&newTempTokens, strippedToken, x);
                        }
                    }
                break;
                case get_else_if_statements:
                    if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "fi") && ifEndCount == 0) {
                        //add the remaining unparsed token to the new token
                        isParsing = T;
                        if((x+1) < strippedToken.tokenCount) {
                            //rebuild and reparse the token
                            parserState = rebuild_tokens;
                        } else {
                            parserState = get_start;
                            strippedToken = newTempTokens;
                        }
                    } else if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "e") && ifEndCount == 0) {
                        parserState = get_if_statements_else;
                    } else if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "ef") && ifEndCount == 0) {
                        currentOperation = none_token;
                        parserState = get_if_opening;

                        if(elseIfWithTrue && !elseIfGotStatements) {
                            elseIfGotStatements = T;
                        }
                    } else {

                        //TODO: prohibit declaration of function inside if statement??
                        //TODO: because it should be in a top level statement...

                        if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "if")) {
                            //track that it's not yet the end of the current if statement
                            ifEndCount += 1;
                        }

                        if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "fi")) {
                            ifEndCount -= 1;
                        }

                        if(!ifWithTrue && elseIfWithTrue && !elseIfGotStatements) {
                            updateTemporaryTokens(&newTempTokens, strippedToken, x);
                        }
                    }
                break;
                case get_if_statements:
                    if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "fi") && ifEndCount == 0) {
                        //add the remaining unparsed token to the new token
                        isParsing = T;
                        if((x+1) < strippedToken.tokenCount) {
                            //rebuild and reparse the token
                            parserState = rebuild_tokens;
                        } else {
                            parserState = get_start;
                            strippedToken = newTempTokens;
                        }
                    } else if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "e") && ifEndCount == 0) {
                        parserState = get_if_statements_else;
                    } else if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "ef") && ifEndCount == 0) {
                        currentOperation = none_token;
                        elseIfMode = T;
                        parserState = get_if_opening;
                    } else {

                        if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "if")) {
                            //track that it's not yet the end of the current if statement
                            ifEndCount += 1;
                        }

                        if(strippedToken.tokens[x].tokenType == keyword_token && !strcmp(strippedToken.tokens[x].tokenValue, "fi")) {
                            ifEndCount -= 1;
                        }

                        if(ifWithTrue) {
                            updateTemporaryTokens(&newTempTokens, strippedToken, x);
                        }
                    }
                break;
                case get_function_open_parenthesis:
                    if(strippedToken.tokens[x].tokenType == open_parenthesis_token) {
                        parserState = get_function_parameters;
                    } else {
                        intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Expecting '('", strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_function_parameters:
                    if(strippedToken.tokens[x].tokenType == close_parenthesis_token) {
                        
                        //check first if # of arguments matched
                        if(argumentArray.argumentCount != globalFunctionArray.functions[functionPosition].argumentArray.argumentCount) {
                            intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Argument count didn't matched", strippedToken.tokens[x].fileName);
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }

                        if(globalFunctionArray.functions[functionPosition].isSystem) {
                            //execute system defined function
                            globalFunctionArray.functions[functionPosition].execute(argumentArray, &intFunctionReturn, &funcReturn);
                            //set return value & type
                            globalFunctionArray.functions[functionPosition].functionReturn = funcReturn;
                        } else {

                            //generate a random scope name
                            strcpy(tempChar, "");
                            rand_str(tempChar, 10);
                            strcat(tempChar, globalFunctionArray.functions[functionPosition].functionName);
                            //add global variables to local variables below (by scope name)
                            copyVariable(tempChar);
                            //convert function arguments to local variable below (by scope name)
                            initArgument(argumentArray, functionPosition, tempChar);
                            //execute user defined function     
                            intFunctionReturn = parseToken(globalFunctionArray.functions[functionPosition].tokens, F, F, &willBreak, tempChar, &funcReturn, &loopReturn);
                            //set the function return value & type below
                            globalFunctionArray.functions[functionPosition].functionReturn = funcReturn;
                            //cleanup local first variables below (by scope name)
                            cleanVariable();
                        }

                        if(intFunctionReturn > 0) {
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }

                        //executed the function in assigment operation
                        if(isFunctionAssignmentMode) {
                            isFunctionAssignmentMode = F;
                            //assign function return to variable
                            
                            if(!isFunctionAssignmentUpdate) {
                                //set variable
                                switch(globalFunctionArray.functions[functionPosition].functionReturn.returnType) {
                                    case ret_string_type:
                                        globalVariableArray.variables[lastVariablePosition].variable_type = var_string_type;
                                        strcpy(globalVariableArray.variables[lastVariablePosition].string_value, globalFunctionArray.functions[functionPosition].functionReturn.string_value);
                                    break;
                                    case ret_integer_type:
                                        globalVariableArray.variables[lastVariablePosition].variable_type = var_integer_type;
                                        globalVariableArray.variables[lastVariablePosition].integer_value = globalFunctionArray.functions[functionPosition].functionReturn.integer_value;
                                    break;
                                    case ret_float_type:
                                        globalVariableArray.variables[lastVariablePosition].variable_type = var_float_type;
                                        globalVariableArray.variables[lastVariablePosition].float_value = globalFunctionArray.functions[functionPosition].functionReturn.float_value;
                                    break;
                                    default:
                                        globalVariableArray.variables[lastVariablePosition].variable_type = var_none_type;
                                }
                            } else {
                                //update variable
                                isFunctionAssignmentUpdate = F;
                                strcpy(tempChar, ""); //clear temp char

                                switch(globalVariableArray.variables[lastVariablePosition].variable_type) {
                                    case var_float_type:
                                    case var_integer_type:
                                        switch(globalFunctionArray.functions[functionPosition].functionReturn.returnType) {
                                            case ret_string_type:
                                                if(currentOperation == plus_token) {
                                                    strcpy(tempChar, "");
                                                    if(globalVariableArray.variables[lastVariablePosition].variable_type == var_integer_type) {
                                                        snprintf(tempChar, TITIK_CHAR_PER_LINE, "%ld", globalVariableArray.variables[lastVariablePosition].integer_value);
                                                    } else {
                                                        snprintf(tempChar, TITIK_CHAR_PER_LINE, "%f", globalVariableArray.variables[lastVariablePosition].float_value);
                                                    }
                                                    
                                                    globalVariableArray.variables[lastVariablePosition].variable_type = var_string_type;
                                                    strcpy(globalVariableArray.variables[lastVariablePosition].string_value, tempChar);

                                                    strcpy(tempChar, globalFunctionArray.functions[functionPosition].functionReturn.string_value);
                                                    strcat(globalVariableArray.variables[lastVariablePosition].string_value, tempChar);
                                                } else {
                                                    intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Invalid operation", strippedToken.tokens[x].fileName);
                                                    freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                                    return intFunctionReturn;
                                                }
                                            break;
                                            case ret_integer_type:
                                                if(currentOperation == plus_token) {
                                                    //addition
                                                    if(globalVariableArray.variables[lastVariablePosition].variable_type == var_integer_type) {
                                                        globalVariableArray.variables[lastVariablePosition].integer_value += globalFunctionArray.functions[functionPosition].functionReturn.integer_value;
                                                    } else {
                                                        globalVariableArray.variables[lastVariablePosition].float_value += (double)globalFunctionArray.functions[functionPosition].functionReturn.integer_value;
                                                    }
                                                } else if(currentOperation == minus_token) {
                                                    //subtraction
                                                    if(globalVariableArray.variables[lastVariablePosition].variable_type == var_integer_type) {
                                                        globalVariableArray.variables[lastVariablePosition].integer_value -= globalFunctionArray.functions[functionPosition].functionReturn.integer_value;
                                                    } else {
                                                        globalVariableArray.variables[lastVariablePosition].float_value -= (double)globalFunctionArray.functions[functionPosition].functionReturn.integer_value;
                                                    }
                                                } else if(currentOperation == multiply_token) {
                                                    //multiplication
                                                    if(globalVariableArray.variables[lastVariablePosition].variable_type == var_integer_type) {
                                                        globalVariableArray.variables[lastVariablePosition].integer_value *= globalFunctionArray.functions[functionPosition].functionReturn.integer_value;
                                                    } else {
                                                        globalVariableArray.variables[lastVariablePosition].float_value *= (double)globalFunctionArray.functions[functionPosition].functionReturn.integer_value;
                                                    }
                                                } else {
                                                    //assume it's division
                                                    if(globalVariableArray.variables[lastVariablePosition].variable_type == var_integer_type) {
                                                        globalVariableArray.variables[lastVariablePosition].integer_value /= globalFunctionArray.functions[functionPosition].functionReturn.integer_value;
                                                    } else {
                                                        globalVariableArray.variables[lastVariablePosition].float_value /= (double)globalFunctionArray.functions[functionPosition].functionReturn.integer_value;
                                                    }
                                                }
                                            break;
                                            case ret_float_type:
                                                if(currentOperation == plus_token) {
                                                    //addition
                                                    if(globalVariableArray.variables[lastVariablePosition].variable_type == var_integer_type) {
                                                        globalVariableArray.variables[lastVariablePosition].integer_value += (long int)globalFunctionArray.functions[functionPosition].functionReturn.float_value;
                                                    } else {
                                                        globalVariableArray.variables[lastVariablePosition].float_value += globalFunctionArray.functions[functionPosition].functionReturn.float_value;
                                                    }
                                                } else if(currentOperation == minus_token) {
                                                    //subtraction
                                                    if(globalVariableArray.variables[lastVariablePosition].variable_type == var_integer_type) {
                                                        globalVariableArray.variables[lastVariablePosition].integer_value -= (long int)globalFunctionArray.functions[functionPosition].functionReturn.float_value;
                                                    } else {
                                                        globalVariableArray.variables[lastVariablePosition].float_value -= globalFunctionArray.functions[functionPosition].functionReturn.float_value;
                                                    }
                                                } else if(currentOperation == multiply_token) {
                                                    //multiplication
                                                    if(globalVariableArray.variables[lastVariablePosition].variable_type == var_integer_type) {
                                                        globalVariableArray.variables[lastVariablePosition].integer_value *= (long int)globalFunctionArray.functions[functionPosition].functionReturn.float_value;
                                                    } else {
                                                        globalVariableArray.variables[lastVariablePosition].float_value *= globalFunctionArray.functions[functionPosition].functionReturn.float_value;
                                                    }
                                                } else {
                                                    //assume it's division
                                                    if(globalVariableArray.variables[lastVariablePosition].variable_type == var_integer_type) {
                                                        globalVariableArray.variables[lastVariablePosition].integer_value /= (long int)globalFunctionArray.functions[functionPosition].functionReturn.float_value;
                                                    } else {
                                                        globalVariableArray.variables[lastVariablePosition].float_value /= globalFunctionArray.functions[functionPosition].functionReturn.float_value;
                                                    }
                                                }
                                            break;
                                            default:
                                                intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "You cannot perform an operation with a Nil type", strippedToken.tokens[x].fileName);
                                                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                                return intFunctionReturn;
                                        }
                                    break;
                                    case var_string_type:

                                        if(currentOperation == plus_token) {
                                            switch(globalFunctionArray.functions[functionPosition].functionReturn.returnType) {
                                                case ret_string_type:
                                                    strcat(globalVariableArray.variables[lastVariablePosition].string_value, globalFunctionArray.functions[functionPosition].functionReturn.string_value);
                                                break;
                                                case ret_integer_type:
                                                    snprintf(tempChar, TITIK_CHAR_PER_LINE, "%ld", globalFunctionArray.functions[functionPosition].functionReturn.integer_value);
                                                    strcat(globalVariableArray.variables[lastVariablePosition].string_value, tempChar);
                                                break;
                                                case ret_float_type:
                                                    snprintf(tempChar, TITIK_CHAR_PER_LINE, "%f", globalFunctionArray.functions[functionPosition].functionReturn.float_value);
                                                    strcat(globalVariableArray.variables[lastVariablePosition].string_value, tempChar);
                                                break;
                                                default:
                                                    intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "You cannot perform an operation with a Nil type", strippedToken.tokens[x].fileName);
                                                    freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                                    return intFunctionReturn;
                                            }
                                        } else {
                                            intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Invalid operation", strippedToken.tokens[x].fileName);
                                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                            return intFunctionReturn;
                                        }

                                    break;
                                    default:
                                        //none type
                                        intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "You cannot perform an operation with a Nil type", strippedToken.tokens[x].fileName);
                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                        return intFunctionReturn;
                                }
                            }
                            checkOperationAndSetParser(x, &parserState, strippedToken);
                        } else {
                            parserState = get_start;
                        }

                    } else if(strippedToken.tokens[x].tokenType == string_token || strippedToken.tokens[x].tokenType == float_token || strippedToken.tokens[x].tokenType == integer_token || strippedToken.tokens[x].tokenType == identifier_token) {
                        //add to arguments

                        if(strippedToken.tokens[x].tokenType == identifier_token) {
                            isVariablesExists = F;
                            variablePosition = 0;

                            isVariablesExists = isVariableExists(&variablePosition, strippedToken.tokens[x].tokenValue, currentScope);
                            if(!isVariablesExists) {
                                intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Undefined variable ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                return intFunctionReturn;
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
                        if(strippedToken.tokens[x - 1].tokenType == string_token || strippedToken.tokens[x - 1].tokenType == float_token || strippedToken.tokens[x - 1].tokenType == integer_token || strippedToken.tokens[x - 1].tokenType == identifier_token) {
                            //then ok
                            //nothing to do
                        } else {
                            intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ','", strippedToken.tokens[x].fileName);
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_assignment_or_function:
                    //check if executing function or assigment statement
                    if(strippedToken.tokens[x].tokenType == equals_token) {
                        //assignment 
                        parserState = get_assignment_value;
                    } else if(strippedToken.tokens[x].tokenType == open_parenthesis_token) {
                        //function execution
                        isFunctionsExists = F;
                        functionPosition = 0;

                        isFunctionsExists = isFunctionExists(&functionPosition, currentIdentifier.tokenValue);

                        if(!isFunctionsExists) {
                            intFunctionReturn = unexpected_error(currentIdentifier.tokenLine, currentIdentifier.tokenColumn, "Undefined function ", currentIdentifier.tokenValue, currentIdentifier.fileName);
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }

                        argumentArray.argumentCount = 0;
                        parserState = get_function_parameters;
                    } else {
                        //unexpected token
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_assignment_value:
                    //reset current operation
                    currentOperation = none_token;

                    //check variable if existing and set the variable value here
                    if(strippedToken.tokens[x].tokenType == string_token || strippedToken.tokens[x].tokenType == float_token || strippedToken.tokens[x].tokenType == integer_token || strippedToken.tokens[x].tokenType == identifier_token || strippedToken.tokens[x].tokenType == keyword_token) {
                        //check first if the identifier is existing as function
                        isFunctionsExists = F;
                        functionPosition = 0;

                        isFunctionsExists = isFunctionExists(&functionPosition, currentIdentifier.tokenValue);
                        if(isFunctionsExists) {
                            intFunctionReturn = syntax_error(currentIdentifier.tokenLine, currentIdentifier.tokenColumn, "Identifier is already defined as function", currentIdentifier.fileName);
                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                            return intFunctionReturn;
                        }

                        //check if variable exists
                        isVariablesExists = isVariableExists(&variablePosition, currentIdentifier.tokenValue, currentScope);
                        
                        if(!isVariablesExists) {
                            variablePosition = globalVariableArray.variableCount;
                            globalVariableArray.variableCount += 1;
                        } else {
                            if(globalVariableArray.variables[variablePosition].is_constant) {
                                intFunctionReturn = unexpected_error(currentIdentifier.tokenLine, currentIdentifier.tokenColumn, "Cannot override constant ", currentIdentifier.tokenValue, currentIdentifier.fileName);
                                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                return intFunctionReturn;
                            }
                        }

                        if(strippedToken.tokens[x].tokenType == identifier_token) {
                            checkVar = T;
                            if((x+1) < strippedToken.tokenCount) {
                                if(strippedToken.tokens[x+1].tokenType == open_parenthesis_token) {
                                    checkVar = F;
                                    isFunctionsExists = F;
                                    functionPosition = 0;

                                    isFunctionsExists = isFunctionExists(&functionPosition, strippedToken.tokens[x].tokenValue);

                                    if(isFunctionsExists) {
                                        argumentArray.argumentCount = 0;
                                        parserState = get_function_open_parenthesis;
                                        isFunctionAssignmentMode = T;
                                        isFunctionAssignmentUpdate = F;
                                        lastVariablePosition = variablePosition;
                                    } else {
                                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected keyword ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                        return intFunctionReturn;
                                    }
                                }
                            }

                            if(checkVar) {
                                isVariablesExists = F;
                                variablePosition2 = 0;

                                isVariablesExists = isVariableExists(&variablePosition2, strippedToken.tokens[x].tokenValue, currentScope);
                                if(!isVariablesExists) {
                                    intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Undefined variable ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                    freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                    return intFunctionReturn;
                                }
                            }
                        }

                        if(strippedToken.tokens[x].tokenType == keyword_token) {
                            isFunctionsExists = F;
                            functionPosition = 0;

                            isFunctionsExists = isFunctionExists(&functionPosition, strippedToken.tokens[x].tokenValue);

                            if(isFunctionsExists) {
                                argumentArray.argumentCount = 0;
                                parserState = get_function_open_parenthesis;
                                isFunctionAssignmentMode = T;
                                isFunctionAssignmentUpdate = F;
                                lastVariablePosition = variablePosition;
                            } else {
                                intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected keyword ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                return intFunctionReturn;
                            }                          
                        }

                        //set variable info
                        setVariableInfo(variablePosition, currentIdentifier, currentScope);

                        //set variable type and value
                        if(strippedToken.tokens[x].tokenType == string_token) {
                            globalVariableArray.variables[variablePosition].variable_type = var_string_type;
                            strcpy(globalVariableArray.variables[variablePosition].string_value, strippedToken.tokens[x].tokenValue);
                            checkOperationAndSetParser(x, &parserState, strippedToken);
                        } else if(strippedToken.tokens[x].tokenType == float_token) {
                            globalVariableArray.variables[variablePosition].variable_type = var_float_type;
                            globalVariableArray.variables[variablePosition].float_value = atof(strippedToken.tokens[x].tokenValue);
                            checkOperationAndSetParser(x, &parserState, strippedToken);
                        } else if(strippedToken.tokens[x].tokenType == integer_token) {
                            globalVariableArray.variables[variablePosition].integer_value = atoi(strippedToken.tokens[x].tokenValue);
                            globalVariableArray.variables[variablePosition].variable_type = var_integer_type;
                            checkOperationAndSetParser(x, &parserState, strippedToken);
                        } else if(strippedToken.tokens[x].tokenType == identifier_token) {
                            if(checkVar) {
                                if(globalVariableArray.variables[variablePosition2].variable_type == var_string_type) {
                                    globalVariableArray.variables[variablePosition].variable_type = var_string_type;
                                    strcpy(tempChar, globalVariableArray.variables[variablePosition2].string_value);
                                    strcpy(globalVariableArray.variables[variablePosition].string_value, tempChar);
                                } else if(globalVariableArray.variables[variablePosition2].variable_type == var_integer_type) {
                                    globalVariableArray.variables[variablePosition].integer_value = globalVariableArray.variables[variablePosition2].integer_value;
                                    globalVariableArray.variables[variablePosition].variable_type = var_integer_type;     
                                } else if(globalVariableArray.variables[variablePosition2].variable_type == var_float_type) {
                                    globalVariableArray.variables[variablePosition].variable_type = var_float_type;
                                    globalVariableArray.variables[variablePosition].float_value = globalVariableArray.variables[variablePosition2].float_value;                
                                } else if(globalVariableArray.variables[variablePosition2].variable_type == var_none_type) {
                                    globalVariableArray.variables[variablePosition].variable_type = var_none_type;                
                                }
                                checkOperationAndSetParser(x, &parserState, strippedToken);
                            }
                        }
                        
                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case get_assigment_operation:
                    //get operation
                    currentOperation = strippedToken.tokens[x].tokenType;
                    parserState = update_assigment_value;

                    if(!((x+1) < strippedToken.tokenCount)) {
                        intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unfinished operation", strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                case update_assigment_value:
                    //update variable depending on the operator
                    if(strippedToken.tokens[x].tokenType == string_token || strippedToken.tokens[x].tokenType == float_token || strippedToken.tokens[x].tokenType == integer_token || strippedToken.tokens[x].tokenType == identifier_token || strippedToken.tokens[x].tokenType == keyword_token) {

                        if(strippedToken.tokens[x].tokenType == keyword_token) {
                            isFunctionsExists = F;
                            functionPosition = 0;

                            isFunctionsExists = isFunctionExists(&functionPosition, strippedToken.tokens[x].tokenValue);

                            if(isFunctionsExists) {
                                argumentArray.argumentCount = 0;
                                parserState = get_function_open_parenthesis;
                                isFunctionAssignmentMode = T;
                                isFunctionAssignmentUpdate = T;
                                lastVariablePosition = variablePosition;
                            } else {
                                intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected keyword ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                return intFunctionReturn;
                            }  
                        } else {
                            switch(globalVariableArray.variables[variablePosition].variable_type) {
                                case var_float_type:
                                case var_integer_type:
                                    //handle float and integer type
                                    switch(strippedToken.tokens[x].tokenType) {
                                        case identifier_token:

                                            checkVar = T;
                                            if((x+1) < strippedToken.tokenCount) {
                                                if(strippedToken.tokens[x+1].tokenType == open_parenthesis_token) {
                                                    checkVar = F;
                                                    isFunctionsExists = F;
                                                    functionPosition = 0;

                                                    isFunctionsExists = isFunctionExists(&functionPosition, strippedToken.tokens[x].tokenValue);

                                                    if(isFunctionsExists) {
                                                        argumentArray.argumentCount = 0;
                                                        parserState = get_function_open_parenthesis;
                                                        isFunctionAssignmentMode = T;
                                                        isFunctionAssignmentUpdate = T;
                                                        lastVariablePosition = variablePosition;
                                                    } else {
                                                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected keyword ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                                        return intFunctionReturn;
                                                    } 
                                                }
                                            }

                                            if(checkVar) {
                                                isVariablesExists = F;
                                                variablePosition2 = 0;
                                                strcpy(tempChar, ""); //clear temp char

                                                isVariablesExists = isVariableExists(&variablePosition2, strippedToken.tokens[x].tokenValue, currentScope);
                                                if(!isVariablesExists) {
                                                    intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Undefined variable ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                                    freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                                    return intFunctionReturn;
                                                }

                                                switch(globalVariableArray.variables[variablePosition2].variable_type) {
                                                    case var_string_type:
                                                        if(currentOperation == plus_token) {
                                                            strcpy(tempChar, "");
                                                            if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                                snprintf(tempChar, TITIK_CHAR_PER_LINE, "%ld", globalVariableArray.variables[variablePosition].integer_value);
                                                            } else {
                                                                snprintf(tempChar, TITIK_CHAR_PER_LINE, "%f", globalVariableArray.variables[variablePosition].float_value);
                                                            }
                                                            
                                                            globalVariableArray.variables[variablePosition].variable_type = var_string_type;
                                                            strcpy(globalVariableArray.variables[variablePosition].string_value, tempChar);

                                                            strcpy(tempChar, globalVariableArray.variables[variablePosition2].string_value);
                                                            strcat(globalVariableArray.variables[variablePosition].string_value, tempChar);
                                                        } else {
                                                            intFunctionReturn = unexpected_error(strippedToken.tokens[x-1].tokenLine, strippedToken.tokens[x-1].tokenColumn, "Invalid operation ", strippedToken.tokens[x-1].tokenValue, strippedToken.tokens[x-1].fileName);
                                                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                                            return intFunctionReturn;
                                                        }

                                                    break;
                                                    case var_float_type:
                                                        if(currentOperation == plus_token) {
                                                            //addition
                                                            if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                                globalVariableArray.variables[variablePosition].integer_value += (long int)globalVariableArray.variables[variablePosition2].float_value;
                                                            } else {
                                                                globalVariableArray.variables[variablePosition].float_value += globalVariableArray.variables[variablePosition2].float_value;
                                                            }
                                                        } else if(currentOperation == minus_token) {
                                                            //subtraction
                                                            if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                                globalVariableArray.variables[variablePosition].integer_value -= (long int)globalVariableArray.variables[variablePosition2].float_value;
                                                            } else {
                                                                globalVariableArray.variables[variablePosition].float_value -= globalVariableArray.variables[variablePosition2].float_value;
                                                            }
                                                        } else if(currentOperation == multiply_token) {
                                                            //multiplication
                                                            if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                                globalVariableArray.variables[variablePosition].integer_value *= (long int)globalVariableArray.variables[variablePosition2].float_value;
                                                            } else {
                                                                globalVariableArray.variables[variablePosition].float_value *= globalVariableArray.variables[variablePosition2].float_value;
                                                            }
                                                        } else {
                                                            //assume it's division
                                                            if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                                globalVariableArray.variables[variablePosition].integer_value /= (long int)globalVariableArray.variables[variablePosition2].float_value;
                                                            } else {
                                                                globalVariableArray.variables[variablePosition].float_value /= globalVariableArray.variables[variablePosition2].float_value;
                                                            }
                                                        }
                                                    break;
                                                    case var_integer_type:
                                                        if(currentOperation == plus_token) {
                                                            if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                                globalVariableArray.variables[variablePosition].integer_value += globalVariableArray.variables[variablePosition2].integer_value;
                                                            } else {
                                                                globalVariableArray.variables[variablePosition].float_value += (double)globalVariableArray.variables[variablePosition2].integer_value;
                                                            }
                                                        } else if(currentOperation == minus_token) {
                                                            if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                                globalVariableArray.variables[variablePosition].integer_value -= globalVariableArray.variables[variablePosition2].integer_value;
                                                            } else {
                                                                globalVariableArray.variables[variablePosition].float_value -= (double)globalVariableArray.variables[variablePosition2].integer_value;
                                                            }
                                                        } else if(currentOperation == multiply_token) {
                                                            if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                                globalVariableArray.variables[variablePosition].integer_value *= globalVariableArray.variables[variablePosition2].integer_value;
                                                            } else {
                                                                globalVariableArray.variables[variablePosition].float_value *= (double)globalVariableArray.variables[variablePosition2].integer_value;
                                                            }
                                                        } else {
                                                            if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                                globalVariableArray.variables[variablePosition].integer_value /= globalVariableArray.variables[variablePosition2].integer_value;
                                                            } else {
                                                                globalVariableArray.variables[variablePosition].float_value /= (double)globalVariableArray.variables[variablePosition2].integer_value;
                                                            }
                                                        }
                                                    break;
                                                    default:
                                                        //variable is a none type
                                                        intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "You cannot perform an operation with a Nil type", strippedToken.tokens[x].fileName);
                                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                                        return intFunctionReturn;
                                                }

                                                checkOperationAndSetParser(x, &parserState, strippedToken);
                                            }

                                        break;
                                        case integer_token:
                                        case float_token:
                                            if(currentOperation == plus_token) {
                                                if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                    globalVariableArray.variables[variablePosition].integer_value += atoi(strippedToken.tokens[x].tokenValue);
                                                } else {
                                                    globalVariableArray.variables[variablePosition].float_value += atof(strippedToken.tokens[x].tokenValue);
                                                }
                                            } else if(currentOperation == minus_token) {
                                                if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                    globalVariableArray.variables[variablePosition].integer_value -= atoi(strippedToken.tokens[x].tokenValue);
                                                } else {
                                                    globalVariableArray.variables[variablePosition].float_value -= atof(strippedToken.tokens[x].tokenValue);
                                                }
                                            } else if(currentOperation == multiply_token) {
                                                if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                    globalVariableArray.variables[variablePosition].integer_value *= atoi(strippedToken.tokens[x].tokenValue);
                                                } else {
                                                    globalVariableArray.variables[variablePosition].float_value *= atof(strippedToken.tokens[x].tokenValue);
                                                }
                                            } else {
                                                if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                    globalVariableArray.variables[variablePosition].integer_value /= atoi(strippedToken.tokens[x].tokenValue);
                                                } else {
                                                    globalVariableArray.variables[variablePosition].float_value /= atof(strippedToken.tokens[x].tokenValue);
                                                }
                                            }
                                            checkOperationAndSetParser(x, &parserState, strippedToken);
                                        break;
                                        case string_token:
                                            if(currentOperation == plus_token) {
                                                //if strippedToken is a string then convert variable to string then concat
                                                strcpy(tempChar, "");
                                                if(globalVariableArray.variables[variablePosition].variable_type == var_integer_type) {
                                                    snprintf(tempChar, TITIK_CHAR_PER_LINE, "%ld", globalVariableArray.variables[variablePosition].integer_value);
                                                } else {
                                                    snprintf(tempChar, TITIK_CHAR_PER_LINE, "%f", globalVariableArray.variables[variablePosition].float_value);
                                                }
                                                globalVariableArray.variables[variablePosition].variable_type = var_string_type;
                                                strcpy(globalVariableArray.variables[variablePosition].string_value, tempChar);
                                                strcat(globalVariableArray.variables[variablePosition].string_value, strippedToken.tokens[x].tokenValue);
                                            } else {
                                                intFunctionReturn = unexpected_error(strippedToken.tokens[x-1].tokenLine, strippedToken.tokens[x-1].tokenColumn, "Invalid operation ", strippedToken.tokens[x-1].tokenValue, strippedToken.tokens[x-1].fileName);
                                                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                                return intFunctionReturn;
                                            }
                                            checkOperationAndSetParser(x, &parserState, strippedToken);
                                        break;
                                        default:
                                            intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                            freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                            return intFunctionReturn;
                                    }
                                break;
                                case var_string_type:
                                    //concatenate only

                                    if(currentOperation == plus_token) {
                                        if(strippedToken.tokens[x].tokenType == identifier_token) {
                                            checkVar = T;
                                            if((x+1) < strippedToken.tokenCount) {
                                                if(strippedToken.tokens[x+1].tokenType == open_parenthesis_token) {
                                                    checkVar = F;
                                                    isFunctionsExists = F;
                                                    functionPosition = 0;

                                                    isFunctionsExists = isFunctionExists(&functionPosition, strippedToken.tokens[x].tokenValue);

                                                    if(isFunctionsExists) {
                                                        argumentArray.argumentCount = 0;
                                                        parserState = get_function_open_parenthesis;
                                                        isFunctionAssignmentMode = T;
                                                        isFunctionAssignmentUpdate = T;
                                                        lastVariablePosition = variablePosition;
                                                    } else {
                                                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected keyword ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                                        return intFunctionReturn;
                                                    } 
                                                }
                                            }

                                            if(checkVar) {
                                                //check if existing variable
                                                isVariablesExists = F;
                                                variablePosition2 = 0;
                                                strcpy(tempChar, ""); //clear temp char

                                                isVariablesExists = isVariableExists(&variablePosition2, strippedToken.tokens[x].tokenValue, currentScope);
                                                if(!isVariablesExists) {
                                                    intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Undefined variable ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                                                    freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                                    return intFunctionReturn;
                                                }

                                                switch(globalVariableArray.variables[variablePosition2].variable_type) {
                                                    case var_float_type:
                                                        snprintf(tempChar, TITIK_CHAR_PER_LINE, "%f", globalVariableArray.variables[variablePosition2].float_value);
                                                        strcat(globalVariableArray.variables[variablePosition].string_value, tempChar);
                                                    break;
                                                    case var_string_type:
                                                        strcpy(tempChar, globalVariableArray.variables[variablePosition2].string_value);
                                                        strcat(globalVariableArray.variables[variablePosition].string_value, tempChar);
                                                    break;
                                                    case var_integer_type:
                                                        snprintf(tempChar, TITIK_CHAR_PER_LINE, "%ld", globalVariableArray.variables[variablePosition2].integer_value);
                                                        strcat(globalVariableArray.variables[variablePosition].string_value, tempChar);
                                                    break;
                                                    default:
                                                        //variable is a none type
                                                        intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "You cannot perform an operation with a Nil type", strippedToken.tokens[x].fileName);
                                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                                        return intFunctionReturn;
                                                }
                                                checkOperationAndSetParser(x, &parserState, strippedToken);
                                            }

                                        } else {
                                            strcat(globalVariableArray.variables[variablePosition].string_value, strippedToken.tokens[x].tokenValue);
                                            checkOperationAndSetParser(x, &parserState, strippedToken);
                                        }

                                    } else {
                                        intFunctionReturn = unexpected_error(strippedToken.tokens[x-1].tokenLine, strippedToken.tokens[x-1].tokenColumn, "Invalid operation ", strippedToken.tokens[x-1].tokenValue, strippedToken.tokens[x-1].fileName);
                                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                        return intFunctionReturn;
                                    }

                                break;
                                default:
                                    //none type
                                    intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "You cannot perform an operation with a Nil type", strippedToken.tokens[x].fileName);
                                    freeArrays(&newTempTokens, &argumentArray, &newTokens);
                                    return intFunctionReturn;
                            }
                        }

                    } else {
                        intFunctionReturn = unexpected_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unexpected token ", strippedToken.tokens[x].tokenValue, strippedToken.tokens[x].fileName);
                        freeArrays(&newTempTokens, &argumentArray, &newTokens);
                        return intFunctionReturn;
                    }
                break;
                default:
                    continue;
            }

            if((x+1) == strippedToken.tokenCount && parserState != get_start) {
                intFunctionReturn = syntax_error(strippedToken.tokens[x].tokenLine, strippedToken.tokens[x].tokenColumn, "Unfinished statement", strippedToken.tokens[x].fileName);
                freeArrays(&newTempTokens, &argumentArray, &newTokens);
                return intFunctionReturn;
            }

        }
    }

    freeArrays(&newTempTokens, &argumentArray, &newTokens);
    return 0;
}