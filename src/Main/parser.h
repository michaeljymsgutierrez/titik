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
    update_assigment_value
} ParserState;

TokenArray stripUnwantedToken(TokenArray tokenArray);
int parseToken(TokenArray tokenArray);

#endif