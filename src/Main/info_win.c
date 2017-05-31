/* info.c
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#include "info.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void help(char exeName[]) {
    printf("Usage: %s [-options] filename.ttk\n", exeName);
    printf("\nwhere options include:\n");
    printf("\t-v\tget current version\n");
    printf("\t-h\tprint this usage info\n");
}

void version() {

    printf("%s %s\n", TITIK_APP_NAME, TITIK_STRING_VERSION);
    printf("By: %s\n", TITIK_AUTHOR);
    printf("Operating System: Windows\n");
}