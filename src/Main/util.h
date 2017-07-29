/* util.h
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#ifndef UTIL_H_
#define UTIL_H_

#include <stdlib.h>
#define TITIK_CHAR_PER_LINE 1000
#define T 1
#define F 0

#ifdef _WIN32
#define TITIK_COLOR_NORMAL 15
#define TITIK_COLOR_RED 4
#define TITIK_COLOR_GREEN 2
#define TITIK_COLOR_YELLOW 14
#define TITIK_COLOR_BLUE 1
#define TITIK_COLOR_MAGENTA 5
#define TITIK_COLOR_CYAN 3
#define TITIK_COLOR_WHITE 15
#else
#define TITIK_COLOR_NORMAL "\x1B[0m"
#define TITIK_COLOR_RED "\x1B[31m"
#define TITIK_COLOR_GREEN "\x1B[32m"
#define TITIK_COLOR_YELLOW "\x1B[33m"
#define TITIK_COLOR_BLUE "\x1B[34m"
#define TITIK_COLOR_MAGENTA "\x1B[35m"
#define TITIK_COLOR_CYAN "\x1B[36m"
#define TITIK_COLOR_WHITE "\x1B[37m"
#endif
#define TITIK_LOADED_FILES_INIT_LENGTH 10000

//error list
//TODO: "ALL" error number should be here
#define TOKEN_ERROR 10
#define SYNTAX_ERROR 11

typedef struct {
    char fileName[TITIK_CHAR_PER_LINE];
} LoadedFile;

typedef struct {
    LoadedFile* loadedFiles;
    int loadedFilesCount;
} LoadedFileArray;

int isFileExists(char fileName[]);
char **readSourceFile(char fileName[], int * ret, int * lineCount);
int token_error(int lineNumber, int columnNumber, char description[], char titikFileName[]);
int syntax_error(int lineNumber, int columnNumber, char description[], char titikFileName[]);
int unexpected_error(int lineNumber, int columnNumber, char description[], char tokenValue[], char titikFileName[]);
void rand_str(char *dest, size_t length);
char * escape_string(char str[]);

#endif