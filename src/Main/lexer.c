/* lexer.c
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#include "lexer.h"
#include "reserved.h"
#include "util.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void updateTemporaryTokens(TokenArray * newTempTokens, TokenArray strippedToken, int x) {
    newTempTokens->tokens[newTempTokens->tokenCount].tokenLine = strippedToken.tokens[x].tokenLine;
    newTempTokens->tokens[newTempTokens->tokenCount].tokenColumn = strippedToken.tokens[x].tokenColumn;
    newTempTokens->tokens[newTempTokens->tokenCount].tokenType = strippedToken.tokens[x].tokenType;
    strcpy(newTempTokens->tokens[newTempTokens->tokenCount].tokenValue, strippedToken.tokens[x].tokenValue);
    strcpy(newTempTokens->tokens[newTempTokens->tokenCount].fileName, strippedToken.tokens[x].fileName);
    newTempTokens->tokenCount += 1;    
}

void setTemporaryToken(Token * currentIdentifier, TokenArray strippedToken, int x, TitikTokenType tokenType) {
    currentIdentifier->tokenType = tokenType;
    strcpy(currentIdentifier->tokenValue, strippedToken.tokens[x].tokenValue);
    currentIdentifier->tokenLine = strippedToken.tokens[x].tokenLine;
    currentIdentifier->tokenColumn = strippedToken.tokens[x].tokenColumn;
    strcpy(currentIdentifier->fileName, strippedToken.tokens[x].fileName);
}

void setToken(int * initToken, int * sPos, TokenArray * tokenArray, char val[], int line, int col, TitikTokenType tType, char titikFileName[]) {
    *initToken = F;
    *sPos = 0;
    tokenArray->tokenCount += 1;
    tokenArray->tokens[tokenArray->tokenCount - 1].tokenLine = line;
    tokenArray->tokens[tokenArray->tokenCount - 1].tokenColumn = col;
    tokenArray->tokens[tokenArray->tokenCount - 1].tokenType = tType;
    strcpy(tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue, val);
    strcpy(tokenArray->tokens[tokenArray->tokenCount - 1].fileName, titikFileName);
}

void updateFinalToken(int x, TokenArray * finalTokenArray, TokenArray tokenArray) {
    strcpy(finalTokenArray->tokens[finalTokenArray->tokenCount].tokenValue, tokenArray.tokens[x].tokenValue);
    strcpy(finalTokenArray->tokens[finalTokenArray->tokenCount].fileName, tokenArray.tokens[x].fileName);
    finalTokenArray->tokens[finalTokenArray->tokenCount].tokenType = tokenArray.tokens[x].tokenType;
    finalTokenArray->tokens[finalTokenArray->tokenCount].tokenLine = tokenArray.tokens[x].tokenLine;
    finalTokenArray->tokens[finalTokenArray->tokenCount].tokenColumn = tokenArray.tokens[x].tokenColumn;
    finalTokenArray->tokenCount += 1;
}

int generateToken(char **fileContent, int totalLineCount, TokenArray * tokenArray, char titikFileName[]) {
    TokenizerState tokenizerState = get_word;
    int strPos = 0;
    int isTokenInit = F; //tells if token is already initialized
    char stringOpener = '"';
    int usePeriod = T;
    int withPeriod = F;

    TokenArray finalTokenArray;
    finalTokenArray.tokens = malloc(TITIK_TOKEN_INIT_LENGTH * sizeof(Token));
    finalTokenArray.tokenCount = 0;

    //categorize to keywords,constants,variable etc...
    for(int x=0; x < totalLineCount; x++) {

        //read character by character
        for(int x2=0; x2 < strlen(fileContent[x]); x2++) {

            switch(tokenizerState) {
                case get_word:

                    if(isalpha(fileContent[x][x2]) || fileContent[x][x2] == '_') {
                        //alphabetic or underscore

                        if(tokenArray->tokenCount > 0) {
                            if(tokenArray->tokens[tokenArray->tokenCount - 1].tokenType == integer_token) {
                                strPos = 0;
                                isTokenInit = F;
                            }
                        }

                        if(!isTokenInit) {
                            isTokenInit = T;
                            tokenArray->tokenCount += 1;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenLine = x;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenColumn = x2;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenType = identifier_token;
                            memset(tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue, 0, strlen(tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue));
                            strcpy(tokenArray->tokens[tokenArray->tokenCount - 1].fileName, titikFileName);
                        }

                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue[strPos] = fileContent[x][x2];
                        strPos += 1;

                    } else if(fileContent[x][x2] == '\n') {
                        //new line
                        setToken(&isTokenInit, &strPos, tokenArray, "\\n", x, x2, newline_token, titikFileName);
                    } else if(fileContent[x][x2] == '\t') {
                        //tab
                        setToken(&isTokenInit, &strPos, tokenArray, "\\t", x, x2, tab_token, titikFileName);
                    } else if(fileContent[x][x2] == '\r') {
                        //carriage return
                        setToken(&isTokenInit, &strPos, tokenArray, "\\r", x, x2, carriage_return_token, titikFileName);
                    } else if(fileContent[x][x2] == ' ') {
                        //space
                        setToken(&isTokenInit, &strPos, tokenArray, "' '", x, x2, space_token, titikFileName);
                    } else if(fileContent[x][x2] == '=') {
                        //equals
                        setToken(&isTokenInit, &strPos, tokenArray, "=", x, x2, equals_token, titikFileName);
                    } else if(fileContent[x][x2] == '+') {
                        //plus
                        setToken(&isTokenInit, &strPos, tokenArray, "+", x, x2, plus_token, titikFileName);
                    } else if(fileContent[x][x2] == '-') {
                        //minus
                        setToken(&isTokenInit, &strPos, tokenArray, "-", x, x2, minus_token, titikFileName);
                    } else if(fileContent[x][x2] == '/') {
                        //divide
                        setToken(&isTokenInit, &strPos, tokenArray, "/", x, x2, divide_token, titikFileName);
                    } else if(fileContent[x][x2] == '*') {
                        //multiply
                        setToken(&isTokenInit, &strPos, tokenArray, "*", x, x2, multiply_token, titikFileName);
                    } else if(fileContent[x][x2] == '.') {
                        //period
                        usePeriod = T;
                        withPeriod = F;

                        if(tokenArray->tokenCount > 0) {
                            if(tokenArray->tokens[tokenArray->tokenCount - 1].tokenType == integer_token) {
                                tokenizerState = get_float;
                                usePeriod = F;
                            }
                        }

                        if(usePeriod) {
                            setToken(&isTokenInit, &strPos, tokenArray, ".", x, x2, period_token, titikFileName);
                        }
                    } else if(fileContent[x][x2] == '(') {
                        //open parenthesis
                        setToken(&isTokenInit, &strPos, tokenArray, "(", x, x2, open_parenthesis_token, titikFileName);
                    } else if(fileContent[x][x2] == ')') {
                        //close parenthesis
                        setToken(&isTokenInit, &strPos, tokenArray, ")", x, x2, close_parenthesis_token, titikFileName);

                    } else if(fileContent[x][x2] == '{') {
                        //open bracket
                        setToken(&isTokenInit, &strPos, tokenArray, "{", x, x2, open_bracket_token, titikFileName);
                    } else if(fileContent[x][x2] == '}') {
                        //close bracket
                        setToken(&isTokenInit, &strPos, tokenArray, "}", x, x2, close_bracket_token, titikFileName);

                    } else if(fileContent[x][x2] == '[') {
                        //open braces
                        setToken(&isTokenInit, &strPos, tokenArray, "[", x, x2, open_braces_token, titikFileName);
                    } else if(fileContent[x][x2] == ']') {
                        //close braces
                        setToken(&isTokenInit, &strPos, tokenArray, "]", x, x2, close_braces_token, titikFileName);

                    } else if(fileContent[x][x2] == ':') {
                        //colon
                        setToken(&isTokenInit, &strPos, tokenArray, ":", x, x2, colon_token, titikFileName);

                    } else if(fileContent[x][x2] == ',') {
                        //comma
                        setToken(&isTokenInit, &strPos, tokenArray, ",", x, x2, comma_token, titikFileName);
                    } else if(fileContent[x][x2] == ';') {
                        //semi colon
                        setToken(&isTokenInit, &strPos, tokenArray, ";", x, x2, semi_colon_token, titikFileName);
                    } else if(fileContent[x][x2] == '&') {
                        //ampersand
                        setToken(&isTokenInit, &strPos, tokenArray, "&", x, x2, ampersand_token, titikFileName);
                    } else if(fileContent[x][x2] == '>') {
                        //greater_than
                        setToken(&isTokenInit, &strPos, tokenArray, ">", x, x2, greater_than_token, titikFileName);
                    } else if(fileContent[x][x2] == '<') {
                        //less_than
                        setToken(&isTokenInit, &strPos, tokenArray, "<", x, x2, less_than_token, titikFileName);
                    } else if(fileContent[x][x2] == '|') {
                        //or
                        setToken(&isTokenInit, &strPos, tokenArray, "|", x, x2, or_token, titikFileName);
                    } else if(fileContent[x][x2] == '\'' || fileContent[x][x2] == '"') {
                        //string
                        stringOpener = fileContent[x][x2];
                        isTokenInit = T;
                        tokenArray->tokenCount += 1;
                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenLine = x;
                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenColumn = x2;
                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenType = string_token;
                        strcpy(tokenArray->tokens[tokenArray->tokenCount - 1].fileName, titikFileName);
                        tokenizerState = get_string;
                        strPos = 0;
                    } else if(fileContent[x][x2] == '#') {
                        //start of single line comment

                        isTokenInit = T;
                        tokenArray->tokenCount += 1;
                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenLine = x;
                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenColumn = x2;
                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenType = single_comment_token;
                        strcpy(tokenArray->tokens[tokenArray->tokenCount - 1].fileName, titikFileName);
                        tokenizerState = get_single_comment;
                        strPos = 0;

                    } else if(fileContent[x][x2] == '\\') {
                        //start of multiline comment
                        isTokenInit = T;
                        tokenArray->tokenCount += 1;
                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenLine = x;
                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenColumn = x2;
                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenType = multi_comment_token;
                        strcpy(tokenArray->tokens[tokenArray->tokenCount - 1].fileName, titikFileName);
                        tokenizerState = get_multi_comment;
                        strPos = 0;
                    } else if(isdigit(fileContent[x][x2])) {
                        //integer
                        if(!isTokenInit) {
                            isTokenInit = T;
                            tokenArray->tokenCount += 1;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenLine = x;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenColumn = x2;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenType = integer_token;
                            strcpy(tokenArray->tokens[tokenArray->tokenCount - 1].fileName, titikFileName);
                        }

                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue[strPos] = fileContent[x][x2];
                        strPos += 1;
                    } else {
                        return token_error(x, x2, "Invalid token", titikFileName);
                    }

                break;
                case get_float:

                    if(isdigit(fileContent[x][x2])) {
                        if(!withPeriod) {
                            withPeriod = T;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue[strPos] = '.';
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenType = float_token;
                            strcpy(tokenArray->tokens[tokenArray->tokenCount - 1].fileName, titikFileName);
                            strPos += 1;
                        }

                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue[strPos] = fileContent[x][x2];
                        strPos += 1;
                    } else {

                        if(!withPeriod) {
                            setToken(&isTokenInit, &strPos, tokenArray, ".", x, x2 - 1, period_token, titikFileName);
                        }

                        //reset to get_word
                        tokenizerState = get_word;
                        isTokenInit = F;
                        strPos = 0;

                        //repeat the process for get_word but with modification
                        if(isalpha(fileContent[x][x2]) || fileContent[x][x2] == '_') {
                            //alphabetic or underscore

                            isTokenInit = T;
                            tokenArray->tokenCount += 1;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenLine = x;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenColumn = x2;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenType = identifier_token;
                            memset(tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue, 0, strlen(tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue));
                            strcpy(tokenArray->tokens[tokenArray->tokenCount - 1].fileName, titikFileName);
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue[strPos] = fileContent[x][x2];
                            strPos += 1;

                        } else if(fileContent[x][x2] == '\n') {
                            //new line
                            setToken(&isTokenInit, &strPos, tokenArray, "\\n", x, x2, newline_token, titikFileName);
                        } else if(fileContent[x][x2] == '\t') {
                            //tab
                            setToken(&isTokenInit, &strPos, tokenArray, "\\t", x, x2, tab_token, titikFileName);
                        } else if(fileContent[x][x2] == '\r') {
                            //carriage return
                            setToken(&isTokenInit, &strPos, tokenArray, "\\r", x, x2, carriage_return_token, titikFileName);                            
                        } else if(fileContent[x][x2] == ' ') {
                            //space
                            setToken(&isTokenInit, &strPos, tokenArray, "' '", x, x2, space_token, titikFileName);
                        } else if(fileContent[x][x2] == '=') {
                            //equals
                            setToken(&isTokenInit, &strPos, tokenArray, "=", x, x2, equals_token, titikFileName);
                        } else if(fileContent[x][x2] == '+') {
                            //plus
                            setToken(&isTokenInit, &strPos, tokenArray, "+", x, x2, plus_token, titikFileName);
                        } else if(fileContent[x][x2] == '-') {
                            //minus
                            setToken(&isTokenInit, &strPos, tokenArray, "-", x, x2, minus_token, titikFileName);
                        } else if(fileContent[x][x2] == '/') {
                            //divide
                            setToken(&isTokenInit, &strPos, tokenArray, "/", x, x2, divide_token, titikFileName);
                        } else if(fileContent[x][x2] == '*') {
                            //multiply
                            setToken(&isTokenInit, &strPos, tokenArray, "*", x, x2, multiply_token, titikFileName);
                        } else if(fileContent[x][x2] == '.') {
                            //period
                            setToken(&isTokenInit, &strPos, tokenArray, ".", x, x2, period_token, titikFileName);
                        } else if(fileContent[x][x2] == '(') {
                            //open parenthesis
                            setToken(&isTokenInit, &strPos, tokenArray, "(", x, x2, open_parenthesis_token, titikFileName);
                        } else if(fileContent[x][x2] == ')') {
                            //close parenthesis
                            setToken(&isTokenInit, &strPos, tokenArray, ")", x, x2, close_parenthesis_token, titikFileName);

                        } else if(fileContent[x][x2] == '{') {
                            //open bracket
                            setToken(&isTokenInit, &strPos, tokenArray, "{", x, x2, open_bracket_token, titikFileName);
                        } else if(fileContent[x][x2] == '}') {
                            //close bracket
                            setToken(&isTokenInit, &strPos, tokenArray, "}", x, x2, close_bracket_token, titikFileName);

                        } else if(fileContent[x][x2] == '[') {
                            //open braces
                            setToken(&isTokenInit, &strPos, tokenArray, "[", x, x2, open_braces_token, titikFileName);
                        } else if(fileContent[x][x2] == ']') {
                            //close braces
                            setToken(&isTokenInit, &strPos, tokenArray, "]", x, x2, close_braces_token, titikFileName);

                        } else if(fileContent[x][x2] == ':') {
                            //colon
                            setToken(&isTokenInit, &strPos, tokenArray, ":", x, x2, colon_token, titikFileName);
                        } else if(fileContent[x][x2] == ',') {
                            //comma
                            setToken(&isTokenInit, &strPos, tokenArray, ",", x, x2, comma_token, titikFileName);
                        } else if(fileContent[x][x2] == ';') {
                            //semi colon
                            setToken(&isTokenInit, &strPos, tokenArray, ";", x, x2, semi_colon_token, titikFileName);
                        } else if(fileContent[x][x2] == '&') {
                            //ampersand
                            setToken(&isTokenInit, &strPos, tokenArray, "&", x, x2, ampersand_token, titikFileName);
                        } else if(fileContent[x][x2] == '>') {
                            //greater_than
                            setToken(&isTokenInit, &strPos, tokenArray, ">", x, x2, greater_than_token, titikFileName);
                        } else if(fileContent[x][x2] == '<') {
                            //less_than
                            setToken(&isTokenInit, &strPos, tokenArray, "<", x, x2, less_than_token, titikFileName);
                        } else if(fileContent[x][x2] == '|') {
                            //or
                            setToken(&isTokenInit, &strPos, tokenArray, "|", x, x2, or_token, titikFileName);
                        } else if(fileContent[x][x2] == '\'' || fileContent[x][x2] == '"') {
                            //string
                            stringOpener = fileContent[x][x2];
                            isTokenInit = T;
                            tokenArray->tokenCount += 1;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenLine = x;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenColumn = x2;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenType = string_token;
                            strcpy(tokenArray->tokens[tokenArray->tokenCount - 1].fileName, titikFileName);
                            tokenizerState = get_string;
                        } else if(fileContent[x][x2] == '#') {
                            //start of single line comment

                            isTokenInit = T;
                            tokenArray->tokenCount += 1;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenLine = x;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenColumn = x2;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenType = single_comment_token;
                            strcpy(tokenArray->tokens[tokenArray->tokenCount - 1].fileName, titikFileName);
                            tokenizerState = get_single_comment;

                        } else if(fileContent[x][x2] == '\\') {
                            //start of multiline comment
                            isTokenInit = T;
                            tokenArray->tokenCount += 1;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenLine = x;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenColumn = x2;
                            tokenArray->tokens[tokenArray->tokenCount - 1].tokenType = multi_comment_token;
                            strcpy(tokenArray->tokens[tokenArray->tokenCount - 1].fileName, titikFileName);
                            tokenizerState = get_multi_comment;
                        } else {
                            return token_error(x, x2, "Invalid token", titikFileName);
                        }

                    }

                break;
                case get_string:

                    if(fileContent[x][x2] == stringOpener) {
                        tokenizerState = get_word;

                        if(fileContent[x][x2] == '"') {
                            setToken(&isTokenInit, &strPos, tokenArray, "\"", x, x2, close_string_token, titikFileName);
                        } else {
                            setToken(&isTokenInit, &strPos, tokenArray, "'", x, x2, close_string_token, titikFileName);
                        }
                        
                    } else {
                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue[strPos] = fileContent[x][x2];
                        strPos += 1;
                    }

                break;
                case get_single_comment:
                
                    if(fileContent[x][x2] == '\n') {
                        tokenizerState = get_word;
                        setToken(&isTokenInit, &strPos, tokenArray, "\\n", x, x2, newline_token, titikFileName);
                    } else {
                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue[strPos] = fileContent[x][x2];
                        strPos += 1;
                    }
                
                break;
                case get_multi_comment:

                    if(fileContent[x][x2] == '\\') {
                        tokenizerState = get_word;
                        setToken(&isTokenInit, &strPos, tokenArray, "\\", x, x2, close_multi_comment_token, titikFileName);
                    } else {
                        tokenArray->tokens[tokenArray->tokenCount - 1].tokenValue[strPos] = fileContent[x][x2];
                        strPos += 1;
                    }

                break;
                default:
                    continue;
            }
        }
    }

    //TOKEN CLEANUP (Convert identifier to keyword if existing in reserved words)
    for(int x=0; x < tokenArray->tokenCount; x++) {
        if(tokenArray->tokens[x].tokenType == identifier_token) {
            if(isReservedWord(tokenArray->tokens[x].tokenValue)) {
                tokenArray->tokens[x].tokenType = keyword_token;
            }
        } else if(tokenArray->tokens[x].tokenType == string_token) {
            if(!((x+1) < tokenArray->tokenCount)) {
                return token_error(tokenArray->tokens[x].tokenLine, tokenArray->tokens[x].tokenColumn, "Expected closing of string", titikFileName);
            }
        } else if(tokenArray->tokens[x].tokenType == multi_comment_token) {
            if(!((x+1) < tokenArray->tokenCount)) {
                return token_error(tokenArray->tokens[x].tokenLine, tokenArray->tokens[x].tokenColumn, "Expected closing of multi line comment", titikFileName);
            }
        }

        if(x != 0 && (tokenArray->tokens[x].tokenType == float_token || tokenArray->tokens[x].tokenType == integer_token)) {

            if(tokenArray->tokens[x - 1].tokenType == minus_token) {
                //set to negative number
                strcat(finalTokenArray.tokens[finalTokenArray.tokenCount - 1].tokenValue, tokenArray->tokens[x].tokenValue);
                finalTokenArray.tokens[finalTokenArray.tokenCount - 1].tokenType = tokenArray->tokens[x].tokenType;
            } else {
                updateFinalToken(x, &finalTokenArray, *tokenArray);
            }

        } else {
            updateFinalToken(x, &finalTokenArray, *tokenArray);
        }
    }

    *tokenArray = finalTokenArray;

    //just for debugging
    //dumpToken(*tokenArray);
    //end just for debugging

    return 0;
}