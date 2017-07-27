/* interactive.c
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#include <stdio.h>
#include <string.h>
#include "interactive.h"
#include "info.h"
#include "util.h"
#include "lexer.h"
#include "debug.h"

void interactive_shell() {
    char userInput[TITIK_CHAR_PER_LINE];
    char indicator[TITIK_CHAR_PER_LINE];
    char **inputStr;
    int lineCount = 1;
    int functionReturn;
    TokenArray tokenArray;

    strcpy(indicator, ">>>");

    printf("%s %s\n", TITIK_APP_NAME, TITIK_STRING_VERSION);
    printf("To exit, press ^C\n");

    while(T) {
        tokenArray.tokens = malloc(TITIK_TOKEN_INIT_LENGTH * sizeof(Token));
        tokenArray.tokenCount = 0;

        printf("%s ", indicator);
        fgets(userInput, TITIK_CHAR_PER_LINE, stdin);

        inputStr = malloc(lineCount * sizeof(char*));
        inputStr[0] = malloc(TITIK_CHAR_PER_LINE * sizeof(char));
        strcpy(inputStr[0], userInput);

        generateToken(inputStr, lineCount, &tokenArray, "interactive_shell");

        //dumpToken(tokenArray);
        free(tokenArray.tokens);
        free(inputStr[0]);
        free(inputStr);
    }
}