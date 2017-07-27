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
    int ifEndCount = 0;
    int fdEndCount = 0;
    int flEndCount = 0;

    printf("%s %s\n", TITIK_APP_NAME, TITIK_STRING_VERSION);
    printf("To exit, press ^C\n");

    while(T) {
        if(!isContinue) {
            strcpy(indicator, ">>>");
        } else {
            strcpy(indicator, "...");
        }

        tokenArray.tokens = malloc(TITIK_TOKEN_INIT_LENGTH * sizeof(Token));
        tokenArray.tokenCount = 0;
        ifEndCount = 0;
        fdEndCount = 0;
        flEndCount = 0;
        lineCount += 1;
        printf("%s ", indicator);
        fgets(userInput, TITIK_CHAR_PER_LINE, stdin);
        
        if(!isContinue) {
            inputStr = malloc(lineCount * sizeof(char*));
        }

        inputStr[lineCount - 1] = malloc(TITIK_CHAR_PER_LINE * sizeof(char));
        strcpy(inputStr[lineCount - 1], userInput);
        generateToken(inputStr, lineCount, &tokenArray, "interactive_shell");
        
        for(int x=0; x < tokenArray.tokenCount; x++) {
            if(!strcmp(tokenArray.tokens[x].tokenValue, "if")) {
                ifEndCount += 1;
            }
            if(!strcmp(tokenArray.tokens[x].tokenValue, "fi")) {
                ifEndCount -= 1;
            }
            if(!strcmp(tokenArray.tokens[x].tokenValue, "fd")) {
                fdEndCount += 1;
            }
            if(!strcmp(tokenArray.tokens[x].tokenValue, "df")) {
                fdEndCount -= 1;
            }
            if(!strcmp(tokenArray.tokens[x].tokenValue, "fl")) {
                flEndCount += 1;
            }
            if(!strcmp(tokenArray.tokens[x].tokenValue, "lf")) {
                flEndCount -= 1;
            }
        }

        if(ifEndCount > 0 || fdEndCount > 0 || flEndCount > 0) {
            isContinue = T;
        } else {
            isContinue = F;
        }

        if(!isContinue) {
            parseToken(tokenArray, F, T, &needBreak, TITIK_MAIN_SCOPE_NAME, &funcReturn, &gotReturn);
            lineCount = 0;
            free(inputStr);
        }

        free(tokenArray.tokens);
    }
}