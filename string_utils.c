#include "string_utils.h"


int read_line(char* str)
{
	int i = 0;
	int c;

	if (str == NULL)
		return -1;

	while (1)
	{
		c  = getchar();
		if (c == EOF || c == '\n')
		{
			str[i] = '\0';
			return i;
		}
		else
			str[i] = c;
		
		i++;
	}
}


int split_string(char* str, char* tokens[], const char* delim)
{
	char* tok = strtok(strdup(str), delim);
	
	int n = 0;

	if (tok == NULL)
	{
		tokens[0] = str;
		n = 1;
	}
	else
	{
		while (tok != NULL)
		{
			tokens[n] = tok;
			n++;
			tok = strtok(NULL, delim);
		}
	}
	
	tokens[n] = NULL;

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
