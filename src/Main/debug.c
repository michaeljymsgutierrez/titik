/* debug.c
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#include "lexer.h"
#include "functions.h"
#include <stdio.h>

void dumpToken(TokenArray tokenArray) {
    printf("=========================\n");

    for(int x=0; x < tokenArray.tokenCount; x++) {
        printf("Token Type: %s\n", TokenTypeString[tokenArray.tokens[x].tokenType]);
        printf("Line #: %d\n", tokenArray.tokens[x].tokenLine + 1);
        printf("Column #: %d\n", tokenArray.tokens[x].tokenColumn + 1);
        printf("Value: %s\n", tokenArray.tokens[x].tokenValue);
        printf("=========================\n");
    }
}

void dumpVariable(VariableArray variableArray) {
    printf("=========================\n");

    for(int x=0; x < variableArray.variableCount; x++) {
        printf("Variable Type: %s\n", variableTypeString[variableArray.variables[x].variable_type]);
        printf("Variable Name: %s\n", variableArray.variables[x].name);
        printf("Scope Name: %s\n", variableArray.variables[x].scope_name);

        switch(variableArray.variables[x].variable_type) {
            case var_string_type:
                printf("Variable Value: %s\n", variableArray.variables[x].string_value);
            break;
            case var_float_type:
                printf("Variable Value: %f\n", variableArray.variables[x].float_value);
            break;
            case var_integer_type:
                printf("Variable Value: %ld\n", variableArray.variables[x].integer_value);
            break;
            default:
                continue;
        }
        printf("Constant: %s\n", (variableArray.variables[x].is_constant)?"True":"False");

        printf("=========================\n");
    }    
}