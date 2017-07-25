/* util.c
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern LoadedFileArray globalLoadedFileArray;

int isFileExists(char fileName[]) {
    int isExists = F;
    int strCompRet = 0;

    for(int x2=0;x2 < globalLoadedFileArray.loadedFilesCount; x2++) {
        strCompRet = strcmp(fileName, globalLoadedFileArray.loadedFiles[x2].fileName);

        if(!(strCompRet < 0) && !(strCompRet > 0)) {
            isExists = T;
            break;
        }
    }

    return isExists;
}

char **readSourceFile(char fileName[], int * ret, int * lineCount) {
    FILE *sourceFile;
    char fileBuffer[TITIK_CHAR_PER_LINE];
    *lineCount = 0;
    char **retStr;
    int counter = 0;

    //add to loadedfilearray
    strcpy(globalLoadedFileArray.loadedFiles[globalLoadedFileArray.loadedFilesCount].fileName, fileName);
    globalLoadedFileArray.loadedFilesCount += 1;

    sourceFile = fopen(fileName, "r");

    if(!sourceFile) {
        printf("Error: no such file: '%s'\n", fileName);
        *ret = 1;
    } else {

        while(fgets(fileBuffer, TITIK_CHAR_PER_LINE, sourceFile) != NULL) {
            *lineCount += 1;
        }

        fseek(sourceFile, 0, SEEK_SET);
        retStr = malloc(*lineCount * sizeof(char*));

        while(fgets(fileBuffer, TITIK_CHAR_PER_LINE, sourceFile) != NULL) {
            retStr[counter] = malloc(TITIK_CHAR_PER_LINE * sizeof(char));
            strcpy(retStr[counter], fileBuffer);
            counter += 1;
        }

        fclose(sourceFile);
        *ret = 0;

    }

    return retStr;
}

int token_error(int lineNumber, int columnNumber, char description[], char titikFileName[]) {
    printf("Token error on line number %d and column number %d, ", lineNumber + 1, columnNumber + 1);
    printf("Error description: %s, ", description);
    printf("Filename: %s\n", titikFileName);
    return TOKEN_ERROR;
}

int syntax_error(int lineNumber, int columnNumber, char description[], char titikFileName[]) {
    printf("Syntax error on line number %d and column number %d, ", lineNumber + 1, columnNumber + 1);
    printf("Error description: %s, ", description);
    printf("Filename: %s\n", titikFileName);
    return SYNTAX_ERROR;
}

int unexpected_error(int lineNumber, int columnNumber, char description[], char tokenValue[], char titikFileName[]) {
    char errorMessage1[TITIK_CHAR_PER_LINE], errorMessage2[TITIK_CHAR_PER_LINE];

    strcpy(errorMessage1, description);
    strcpy(errorMessage2, "'");
    strcat(errorMessage2, tokenValue);
    strcat(errorMessage2, "'");
    strcat(errorMessage1, errorMessage2);
    return syntax_error(lineNumber, columnNumber, errorMessage1, titikFileName);
}

//from: https://stackoverflow.com/questions/15767691/whats-the-c-library-function-to-generate-random-string
void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

char * replace_crlf(char str[]) {
    static char retStr[TITIK_CHAR_PER_LINE];
    int ctr = 0;
    int opened = F;
    for(int x=0; x < strlen(str); x++) {
        if(opened) {
            if(str[x] == 'n') {
                retStr[ctr] = '\n';
                ctr += 1;
            } else if(str[x] == 't') {
                retStr[ctr] = '\t';
                ctr += 1;
            } else {
                retStr[ctr] = '\\';
                ctr += 1;
                retStr[ctr] = str[x];
                ctr += 1;
            }
            opened = F;
        } else {
            if(str[x] == '\\') {
                opened = T;
            } else {
                retStr[ctr] = str[x];
                ctr += 1;
            }
        }
    }

    if(ctr > 0) {
        retStr[ctr + 1] = 0;
    } else {
        retStr[ctr] = 0;
    }

    return retStr;
}