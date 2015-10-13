/*Basic UNIX Shell Simulator */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFFER_SIZE 80 //max 80 chars input from user

void print_prompt()
{
	printf("%s ","$");
}

char *get_input()
{
	char *command = malloc(BUFFER_SIZE);
	if(fgets(command,BUFFER_SIZE,stdin))
		return command;
	else
		return NULL;
}

char **parse_command(char *command)
{
	return NULL;
}

int execute_command(char **command_args)
{
	return 1;
}

int main(void)
{
	int command_status = 1; //if 0 exit
	char *command;
	char **command_args;
	while(command_status)
	{
		print_prompt();
		if((command = get_input())==NULL)
		{
			printf("Read Error\n");
			continue;
		}
		command_args = parse_command(command);
		command_status = execute_command(command_args);
		free(command);
		command = NULL;
	}
	return 0;
}