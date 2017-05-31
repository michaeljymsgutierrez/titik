/* debug.h
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#ifndef DEBUG_H_
#define DEBUG_H_

#include "lexer.h"
#include "functions.h"

void dumpToken(TokenArray tokenArray);
void dumpVariable(VariableArray variableArray);

#endif