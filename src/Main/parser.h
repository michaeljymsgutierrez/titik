/* parser.h
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"
#include "functions.h"

typedef enum {
    get_start,
    get_function_open_parenthesis,
    get_function_parameters,
    get_assignment_or_function,
    get_assignment_value,
    get_assigment_operation,
    update_assigment_value,
    get_if_opening,
    get_if_expression1,
    get_if_operator,
    get_if_expression2_or_end
} ParserState;

void setTemporaryToken(Token * currentIdentifier, TokenArray strippedToken, int x, TokenType tokenType);
TokenArray stripUnwantedToken(TokenArray tokenArray);
int parseToken(TokenArray tokenArray);

#endif