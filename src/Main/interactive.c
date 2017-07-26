/* interactive.c
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#include <stdio.h>
#include <string.h>
#include "interactive.h"
#include "info.h"
#include "util.h"

void interactive_shell() {
    char userInput[TITIK_CHAR_PER_LINE];
    char indicator[TITIK_CHAR_PER_LINE];

    strcpy(indicator, ">>>");

    printf("%s %s\n", TITIK_APP_NAME, TITIK_STRING_VERSION);
    printf("To exit, press ^C\n");

    while(T) {
        printf("%s ", indicator);
        fgets(userInput, TITIK_CHAR_PER_LINE, stdin);
    }
}