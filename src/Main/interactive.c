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
#include "parser.h"

void interactive_shell() {
    char userInput[TITIK_CHAR_PER_LINE];
    char indicator[TITIK_CHAR_PER_LINE];
    int functionReturn;
    int needBreak = F;
    int gotReturn = F;
    FunctionReturn funcReturn;
    char **inputStr;
    int lineCount = 0;
    TokenArray tokenArray;
    int isContinue = F;

    printf("%s %s\n", TITIK_APP_NAME, TITIK_STRING_VERSION);
    printf("To exit, press ^C\n");

    while(T) {
        /*
        tokenArray.tokens = malloc(TITIK_TOKEN_INIT_LENGTH * sizeof(Token));
        tokenArray.tokenCount = 0;

        strcpy(userInput, "");
        printf("%s ", indicator);
        fgets(userInput, TITIK_CHAR_PER_LINE, stdin);

        inputStr = malloc(lineCount * sizeof(char*));
        inputStr[0] = malloc(TITIK_CHAR_PER_LINE * sizeof(char));
        strcpy(inputStr[0], userInput);

        generateToken(inputStr, lineCount, &tokenArray, "interactive_shell");
        parseToken(tokenArray, F, T, &needBreak, TITIK_MAIN_SCOPE_NAME, &funcReturn, &gotReturn);
        //dumpToken(tokenArray);
        free(tokenArray.tokens);
        free(inputStr[0]);
        free(inputStr);
        */
        if(!isContinue) {
            strcpy(indicator, ">>>");
            tokenArray.tokens = malloc(TITIK_TOKEN_INIT_LENGTH * sizeof(Token));
            tokenArray.tokenCount = 0;
        } else {
            strcpy(indicator, "...");
        }

        lineCount += 1;
        printf("%s ", indicator);
        fgets(userInput, TITIK_CHAR_PER_LINE, stdin);
        
        if(!isContinue) {
            inputStr = malloc(lineCount * sizeof(char*));
        }

        inputStr[lineCount - 1] = malloc(TITIK_CHAR_PER_LINE * sizeof(char));
        strcpy(inputStr[lineCount - 1], userInput);
        generateToken(inputStr, lineCount, &tokenArray, "interactive_shell");
        
        //check tokens below (TODO:)

        if(!isContinue) {
            parseToken(tokenArray, F, T, &needBreak, TITIK_MAIN_SCOPE_NAME, &funcReturn, &gotReturn);
            lineCount = 0;
            free(tokenArray.tokens);
            free(inputStr);
        }
    }
}