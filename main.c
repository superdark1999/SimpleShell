#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "string_utils.h"


#define MAX_PIPE 10
#define MAX_ARGC 80
#define BUFFER_SIZE 1024


/**
 * Execute a standard command, support redirection and running concurrently
 * @param {char*} command - the command need to execute
 * @returns {int} 1 is execute successfully, otherwise 0
 */
int execute_command(char* command);

/**
 * Execute a pipe command, support running concurrently
 * @param {char*} command - the pipe command need to execute
 * @returns {int} 1 is execute successfully, otherwise 0
 */
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

	//	Check if command must be run concurrently
	int run_concurrently = 0;
	if (strcmp(args[argc - 1], "&") == 0)
	{
		run_concurrently = 1;
		args[argc - 1] = NULL;
	}

	//	Check redirection
	int less_than_symbol = contains(args, "<");
	int greater_than_symbol = contains(args, ">");
	int input_file = -1;
	int output_file = -1;

	//	Open input file if found "<"
	if (less_than_symbol != -1)
		input_file = open(args[less_than_symbol + 1], O_RDONLY);

	//	Open output file if found "<"
	if (greater_than_symbol != -1)
		output_file = open(args[greater_than_symbol + 1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

	//	Cut out the unnescessary redirection part
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

	//	Create child process and execute command
	__pid_t pid = fork();
	int status;

	if (pid == 0)
	{
		dup2(input_file, STDIN_FILENO);
		dup2(output_file, STDOUT_FILENO);
		close(input_file);
		close(output_file);

		if (run_concurrently)
			printf("\nJob '%s' is executing\n", command);

		int result = execvp(args[0], args);
		exit(EXIT_FAILURE);
	}	
	else if (pid != -1)
	{
		if (!run_concurrently)
		{
			waitpid(pid, &status, WUNTRACED);
			if (status != EXIT_SUCCESS)
			{
				printf("command not found: %s\n", args[0]);
				return 0;
			}
		}
	}
	else
	{
		printf("Fork error!\n");
		return 0;
	}

	return 1;
}



int execute_pipe_command(char* command)
{
	char *child_command[MAX_PIPE];
	char *args[MAX_ARGC];

	//	Check if command must be run concurrently
	int run_concurrently = 0;
	if (strchr(command, '&') != NULL)
	{
		run_concurrently = 1;
		*strchr(command, '&') = '\0';
	}

	int n_child = split_string(command, child_command, "|");

	__pid_t pid = fork();
	int status;

	if (pid == 0)
	{
		int fd[2];
		pipe(fd);

		if (run_concurrently)
			printf("\nJob '%s' is executing\n", command);

		__pid_t child_pid = fork();
		if (child_pid == 0)
		{
			split_string(child_command[0], args, " ");

			dup2(fd[1], STDOUT_FILENO);

			close(fd[0]);
			close(fd[1]);

			execvp(args[0], args);
			exit(EXIT_SUCCESS);
		}
		else if (child_pid != -1)
		{
			split_string(child_command[1], args, " ");

			wait(NULL);

			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			close(fd[1]);

			execvp(args[0], args);
		}
		else
		{
			printf("Fork error!!\n");
		}
		
	}	
	else if (pid != -1)
	{	
		if (!run_concurrently)
		{
			waitpid(pid, &status, WUNTRACED);
		}
	}
	else
	{
		printf("Fork error!!\n");
	}
	

	return 1;
}
