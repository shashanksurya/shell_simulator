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
#include <ctype.h>
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


/*To add spaces after special characters if there are none. This will help
  to make sure our command is always in specific format.*/
char *add_spaces(char *command)
{
	char *cmd = malloc(BUFFER_SIZE);
	char *ptr = command;
	int s = 0;
	while(*ptr!=0)
	{
		//if there is a special character and there is no space
		if(*ptr == '|' || *ptr == '<' || *ptr == '>' || *ptr == '&')
		{
			//no space before
			if(s>0 && cmd[s-1]!=' ')
			{
				cmd[s++] = ' ';
			}
			cmd[s++] = *ptr;
			//no space after
			if(*(ptr + 1)!= ' ')
				cmd[s++] = ' ';
			else if((ptr+1)!=0)
			{
				cmd[s++] = ' ';
				ptr++;
			}
			ptr++;
		}
		else
		{
			cmd[s++] = *ptr;
			ptr++;
		}
	}
	//mark the end of the string
	cmd[s] = '\0';
	strcpy(command,cmd);
	free(cmd);
	return command;
}

//to get the total number of arguments in the command
int get_arg_count(char *command)
{
	int i=0,arg_count = 0;
	while(command[i]!='\0')
	{
		if(command[i] == ' ')
			arg_count++;
		i++;
	}
	return arg_count;
}

//This is where actual raw input goes and comes out formatted
char **parse_command(char *command)
{
	char *cmd,*token;
	char **cmd_args;
	char seperator[] = " \n";
	int i=0;
	command = add_spaces(command);
	cmd_args = (char **)malloc((get_arg_count(command)+1)*sizeof(char *));
	cmd = strdup(command);
	token = strtok(cmd,seperator);
	while(token!=NULL)
	{
		//printf("Token: %s i value: %d\n",token,i);
		cmd_args[i] = (char *)malloc((strlen(token)+1)*sizeof(char *));
		strcpy(cmd_args[i],token);
		token = strtok(NULL,seperator);
		i+=1;
	}
	cmd_args[i] = NULL;
	free(cmd);
	return cmd_args;
}

void cleanup(char **command_args)
{
	char *c;
	int i=0;
	while((c=command_args[i])!=NULL)
	{
		free(c);
		c = NULL;
		i++;
	}
	free(command_args);
}

//Used to remove any trailing space for commands like pwd,ls
//which is unnecessary
char *remove_trailing_space(char *command)
{
	int end = strlen(command);
	if(isspace(command[end-1]))
	{
		printf("Space found\n");
		command[end-1] = '\0';
	}
	return command;
}

int execute_command(char **command_args)
{
	char *cmd = command_args[0];
	char *path = command_args[1];
	char *temp;
	temp = malloc(BUFFER_SIZE);
	remove_trailing_space(cmd);
	//Sort out if there is a pipe or redirection before, else do normally
		if(strcmp(cmd,"cd") == 0)
		{
			//Absolute path
			if(path[0] == '/')
			{
				if(chdir(path)!=0)
				{
					perror(path);
				}
			}
			//Relative path
			else
			{
				//Max 80 char address here
				getcwd(temp,BUFFER_SIZE);
				strcat(temp,"/");
				strcat(temp,path);
				if(chdir(temp)!=0)
				{
					perror(temp);
				}
			}
		}
		else if(strcmp(command_args[0],"set")==0)
		{

		}
		else if(strcmp(command_args[0],"pwd")==0)
		{
			if (getcwd(temp,BUFFER_SIZE)!=NULL)
			{
				printf("%s\n",temp);
			}
			else
				printf("Failed fetching current working directory\n");
		}
		else if(strcmp(command_args[0],"exit")==0)
		{
			cleanup(command_args);
			exit(0);
		}
		else
		{//Pass these to the shell
		printf("Not anything that I know dude\n");
		}
	free(temp);
	return 1;		
}



int main(void)
{
	int command_status = 1,i=0; //if 0 exit
	char *command;
	char **command_args;
	char *temp;
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
		cleanup(command_args);
		command = NULL;
	}
	return 0;
}