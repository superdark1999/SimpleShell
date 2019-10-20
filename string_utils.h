#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/**
 * Read input from stdin
 * @param {char*} str - string to store result
 * @returns {int} length of read string, -1 if read fail
 */
int read_line(char* str);

/**
 * Split a string in to tokens use delimeter.
 * A NULL element will be appended after the last token
 * @param {char*} str - string to split
 * @param {char*[]} tokens - array of string to store result
 * @param {char*} delimeter used to split string
 * @returns {int} number of tokens
 */
int split_string(char* str, char* tokens[], const char* delim);

/**
 * Check if a string array contains a given string 
 * @param {char*[]} strs - string array
 * @param {char*} string to check
 * @returns {int} the position of the given string in string array, -1 if not found
 */
int contains(char* strs[], const char* value);


#endif
