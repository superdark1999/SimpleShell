#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void read_line(char* str);


void print(char** str_arr, int n);


int split_string(char* command, char* args[], const char* delim);


int contains(char* strs[], const char* value);


#endif
