/* reserved.c
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#include "reserved.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

int isReservedWord(char wrd[]) {

    for(int x=0; x < RESERVED_WORD_COUNT; x++) {
        int ret = strcmp(wrd, RESERVED_WORDS[x]);
        if(!(ret < 0) && !(ret > 0)) {
            return T;
        }
    }

    return F;
}