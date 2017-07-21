/* util.h
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#ifndef UTIL_H_
#define UTIL_H_

#define TITIK_CHAR_PER_LINE 1000
#define T 1
#define F 0

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

#endif