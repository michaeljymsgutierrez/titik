/* reserved.h
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#ifndef RESERVED_H_
#define RESERVED_H_

#define RESERVED_WORD_COUNT 15

static const char *RESERVED_WORDS[RESERVED_WORD_COUNT] = {
    "zzz", //sleep
    "p", //print
    "fd", //function
    "df", //end of function declaration
    "fl", //for loop
    "lf", //end of for loop
    "i", //include
    "if", //if
    "fi", //end of if
    "ef", //else if
    "e", //else
    "to", //use in looping statement
    "b", //break loop
    "rtn", //return
    "r" //readline
};

int isReservedWord(char wrd[]);

#endif