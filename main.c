#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "string_utils.h"


#define MAX_PIPE 10
#define MAX_ARGC 80
#define BUFFER_SIZE 1024



int execute_command(char* command);
int execute_pipe_command(char* command);



int main()
{
	char command[BUFFER_SIZE] = "";
	char history[BUFFER_SIZE] = "";

    int should_run = 1;

    while (should_run)
	{   
    	printf("$ ");

		read_line(command);

	   	if (strcmp(command, "!!") == 0)
		{
			if (strcmp(history, "") != 0)
			{
				strcpy(command, history);
				printf("  Execute command: %s\n", command);	
			}
			else
			{
				printf("  No history\n");
				continue;
			}
		}

		if (strcmp(command, "") == 0)
			continue;

		if (strchr(command, '|') == NULL)
		{
			execute_command(command);
			strcpy(history, command);
		}
		else
		{
			execute_pipe_command(command);
			strcpy(history, command);
		}

    }


	return 0;
}



int execute_command(char* command)
{
	char *args[MAX_ARGC];
	int argc = split_string(command, args, " ");

	int less_than_symbol = contains(args, "<");
	int greater_than_symbol = contains(args, ">");
	int input_file = -1;
	int output_file = -1;

	if (less_than_symbol != -1)
		input_file = open(args[less_than_symbol + 1], O_RDONLY);

	if (greater_than_symbol != -1)
		output_file = open(args[greater_than_symbol + 1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

	if (less_than_symbol != -1 && greater_than_symbol == -1)
		args[less_than_symbol] = NULL;
	else if (less_than_symbol == -1 && greater_than_symbol != -1)
		args[greater_than_symbol] = NULL;
	else if (less_than_symbol != -1 && greater_than_symbol != -1)
	{
		int min = less_than_symbol;
		if (greater_than_symbol < less_than_symbol)
			min = greater_than_symbol;
		args[min] = NULL;
	}
		

	__pid_t pid = fork();
	int status;

	if (pid == 0)
	{
		dup2(input_file, STDIN_FILENO);
		dup2(output_file, STDOUT_FILENO);
		close(input_file);
		close(output_file);
		
		execvp(args[0], args);
	}	
	else
	{
		waitpid(pid, &status, WUNTRACED);
	}

	return 1;
}



int execute_pipe_command(char* command)
{
	char *child_command[MAX_PIPE];
	char *args[MAX_ARGC];
	
	int n_child = split_string(command, child_command, "|");

	__pid_t pid = fork();
	int status;

	if (pid == 0)
	{
		int fd[2];
		pipe(fd);

		if (fork() == 0)
		{
			split_string(child_command[0], args, " ");

			dup2(fd[1], STDOUT_FILENO);

			close(fd[0]);
			close(fd[1]);

			execvp(args[0], args);
			exit(EXIT_SUCCESS);
		}
		else
		{
			split_string(child_command[1], args, " ");

			wait(NULL);

			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			close(fd[1]);

			execvp(args[0], args);
		}
	}	
	else
	{
		waitpid(pid, &status, WUNTRACED);
	}

	return 1;
}
