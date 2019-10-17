#include "string_utils.h"


void read_line(char* str)
{
	int i = 0;
	int c;

	if (str == NULL)
		return;

	while (1)
	{
		c  = getchar();
		if (c == EOF || c == '\n')
		{
			str[i] = '\0';
			return;
		}
		else
			str[i] = c;
		
		i++;
	}
}


void print(char** str_arr, int n)
{
	for (int i = 0; i < n; i++)
		printf("    \"%s\"\n", str_arr[i]);
}


int split_string(char* command, char* args[], const char* delim)
{
	char* tok = strtok(strdup(command), delim);
	
	int n = 0;

	if (tok == NULL)
	{
		args[0] = command;
		n = 1;
	}
	else
	{
		while (tok != NULL)
		{
			args[n] = tok;
			n++;
			tok = strtok(NULL, delim);
		}
	}
	
	args[n] = NULL;

	return n;
}


int contains(char* strs[], const char* value)
{
	int i = 0;
	while (strs[i] != NULL)
	{
		if (strcmp(strs[i], value) == 0)
			return i;
		i++;
	}

	return -1;
}
