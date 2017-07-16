/* lexer.h
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#ifndef LEXER_H_
#define LEXER_H_
#include "util.h"

#define TITIK_TOKEN_INIT_LENGTH 50000 //how many token can generate???

typedef enum {
    identifier_token,
    period_token,
    keyword_token,
    space_token,
    open_parenthesis_token,
    close_parenthesis_token,
    single_comment_token,
    multi_comment_token,
    close_multi_comment_token,
    newline_token,
    equals_token,
    string_token,
    close_string_token,
    integer_token,
    float_token,
    plus_token,
    minus_token,
    divide_token,
    multiply_token,
    colon_token,
    semi_colon_token,
    comma_token,
    open_bracket_token,
    close_bracket_token,
    open_braces_token,
    close_braces_token,
    ampersand_token,
    greater_than_token,
    less_than_token,
    or_token,
    exclamation_token,
    tab_token,
    carriage_return_token,
    none_token
} TitikTokenType;

static const char *TitikTokenTypeString[] = {
    "identifier_token",
    "period_token",
    "keyword_token",
    "space_token",
    "open_parenthesis_token",
    "close_parenthesis_token",
    "single_comment_token",
    "multi_comment_token",
    "close_multi_comment_token",
    "newline_token",
    "equals_token",
    "string_token",
    "close_string_token",
    "integer_token",
    "float_token",
    "plus_token",
    "minus_token",
    "divide_token",
    "multiply_token",
    "colon_token",
    "semi_colon_token",
    "comma_token",
    "open_bracket_token",
    "close_bracket_token",
    "open_braces_token",
    "close_braces_token",
    "ampersand_token",
    "greater_than_token",
    "less_than_token",
    "or_token",
    "exclamation_token",
    "tab_token",
    "carriage_return_token",
    "none_token"
};

typedef struct {
    char tokenValue[TITIK_CHAR_PER_LINE];
    char fileName[TITIK_CHAR_PER_LINE];
    TitikTokenType tokenType;
    int tokenLine; //reference for line #
    int tokenColumn; //reference for column #
} Token;

typedef struct {
    Token* tokens;
    int tokenCount;
} TokenArray;

typedef enum {
    get_word,
    get_single_comment,
    get_multi_comment,
    get_string,
    get_float
} TokenizerState;

void updateTemporaryTokens(TokenArray * newTempTokens, TokenArray strippedToken, int x);
void setTemporaryToken(Token * currentIdentifier, TokenArray strippedToken, int x, TitikTokenType tokenType);
void setToken(int * initToken, int * sPos, TokenArray * tokenArray, char val[], int line, int col, TitikTokenType tType, char titikFileName[]);
void updateFinalToken(int x, TokenArray * finalTokenArray, TokenArray tokenArray);
int generateToken(char **fileContent, int totalLineCount, TokenArray * tokenArray, char titikFileName[]);

#endif