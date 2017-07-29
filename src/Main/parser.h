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
    get_if_expression2,
    get_if_operator_or_end,
    get_if_end,
    get_if_statements,
    get_if_statements_else,
    get_else_if_statements,
    get_for_loop_opening,
    get_for_loop_from,
    get_for_loop_to_word,
    get_for_loop_to,
    get_for_loop_end,
    get_for_loop_statements,
    get_function_name,
    get_function_declaration_open_parenthesis,
    get_function_declaration_parameters,
    get_function_body,
    get_return_value,
    rebuild_tokens
} ParserState;

void checkOperationAndSetParser(int x, ParserState * parserState, TokenArray strippedToken);
void freeArrays(TokenArray * newTempTokens, ArgumentArray * argumentArray, TokenArray * newTokens);
int parseToken(TokenArray tokenArray, int isLoop, int stripIt, int * needBreak, char currentScope[], FunctionReturn * thisReturn, int * gotReturn);

#endif