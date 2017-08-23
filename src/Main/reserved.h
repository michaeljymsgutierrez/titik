/* reserved.h
 *
 *  Copyright (C) 2017  Ferdinand Silva
 */
#ifndef RESERVED_H_
#define RESERVED_H_

#define RESERVED_WORD_COUNT 34

static const char *RESERVED_WORDS[RESERVED_WORD_COUNT] = {
    "zzz", //sleep
    "p", //print
    "fd", //function
    "df", //end of function declaration
    "fl", //for loop
    "lf", //end of for loop
    "wl", //while loop
    "lw", //end of while loop
    "i", //include
    "if", //if
    "fi", //end of if
    "ef", //else if
    "e", //else
    "to", //use in looping statement
    "b", //break loop
    "tof", //convert to float
    "toi", //convert to integer
    "tos", //convert to string
    "rtn", //return
    "ex", //exit
    "sc", //set color
    "flcp", //file copy
    "flmv", //file cut
    "flrm", //file remove
    "exe", //execute command
    "r", //readline
    "sac", //system arg count
    "sav", //get system arg values (must be an array)
    "savf", //get first system arg value
    "rndstr", //generate random string
    "rnd", //random
    "genv", //get environment variable
    "senv", //set environment variable
    "as" //get array size
};

int isReservedWord(char wrd[]);

#endif