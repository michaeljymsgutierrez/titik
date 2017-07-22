/* titik.c
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#include "info.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "functions.h"
#include "debug.h"

FunctionArray globalFunctionArray;
VariableArray globalVariableArray;
LoadedFileArray globalLoadedFileArray;

int main(int argc, char **argv) {
    char **fileContent;
    int functionReturn;
    int totalLineCount;
    int needBreak = F;

    TokenArray tokenArray;
    //init token array
    tokenArray.tokens = malloc(TITIK_TOKEN_INIT_LENGTH * sizeof(Token));
    tokenArray.tokenCount = 0;

    //init function array
    globalFunctionArray.functions = malloc(TITIK_FUNCTION_INIT_LENGTH * sizeof(Function));
    globalFunctionArray.functionCount = 0;

    //init variable array
    globalVariableArray.variables = malloc(TITIK_VARIABLE_INIT_LENGTH * sizeof(Variable));
    globalVariableArray.variableCount = 0;

    //init loaded files array
    globalLoadedFileArray.loadedFiles = malloc(TITIK_LOADED_FILES_INIT_LENGTH * sizeof(LoadedFile));
    globalLoadedFileArray.loadedFilesCount = 0;

    //init global functions
    initFunctions(&globalFunctionArray);
    //init global variables & constants
    initVariables(&globalVariableArray);

    if (argc != 2) {
        help(argv[0]);
        return 1;
    }

    if (!strcmp(argv[1], "-v")) {
        version();
    } else if(!strcmp(argv[1], "-h")) {
        help(argv[0]);
    } else {
        //open titik file
        fileContent = readSourceFile(argv[1], &functionReturn, &totalLineCount);
        
        if(functionReturn > 0)
            return functionReturn;

        //generate token
        functionReturn = generateToken(fileContent, totalLineCount, &tokenArray, argv[1]);

        if(functionReturn > 0)
            return functionReturn;

        //parse token
        functionReturn = parseToken(tokenArray, F, T, &needBreak, TITIK_MAIN_SCOPE_NAME);

        if(functionReturn > 0)
            return functionReturn;

        //this is for debugging purpose only
        //dumpVariable(globalVariableArray);
        //end of debugging purpose

        free(fileContent);
        free(tokenArray.tokens);
        free(globalFunctionArray.functions);
        free(globalVariableArray.variables);
    }

    return 0;
}