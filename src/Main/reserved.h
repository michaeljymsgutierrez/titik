/* reserved.h
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#ifndef RESERVED_H_
#define RESERVED_H_

#define RESERVED_WORD_COUNT 7

static const char *RESERVED_WORDS[RESERVED_WORD_COUNT] = {
    "p", //print
    "l", //loop
    "f", //function
    "i", //include
    "if", //if
    "ef", //else if
    "e" //else
};

int isReservedWord(char wrd[]);

#endif