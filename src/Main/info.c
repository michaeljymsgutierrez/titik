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
    printf("\t-i\topen interactive shell\n");
    printf("\t-h\tprint this usage info\n");
}

void version() {
    printf("%s %s\n", TITIK_APP_NAME, TITIK_STRING_VERSION);
    printf("By: %s\n", TITIK_AUTHOR);
    #ifdef _WIN32
    printf("Operating System: Windows\n");
    #else
    int sys_ret = 0;
    FILE *os_file;
    char fileBuffer[100];
    char osInfo[100];
    strcpy(osInfo, "");
    sys_ret = system("uname > os_info");
    
    if(sys_ret == 0) {
        os_file = fopen("os_info", "r");

        while(fgets(fileBuffer, 100, os_file) != NULL) {
            strcat(osInfo, fileBuffer);
        }

        fclose(os_file);
        remove("os_info");
        printf("Operating System: %s\n", osInfo);
    } else {
        printf("Operating System: Unknown\n");
    }
    #endif
}