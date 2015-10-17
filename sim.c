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
#include <signal.h>
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

int check_pipe(char **cmd_args)
{
	int i=0,pipe_count=0;
	for(i=0;cmd_args[i]!=NULL;i++)
	{
		if(strcmp(cmd_args[i],"|")==0)
		{
			pipe_count+=1;
		}
	}
	return pipe_count;
}

char **remove_redirection(char **cmd_args)
{
	int i, j;
	for (i = 0, j = 0; cmd_args[i] != NULL; ++i) {
		char *arg = cmd_args[i];
		if ((strcmp(arg, ">") == 0 || strcmp(arg, "<") == 0)) {
			free(arg); // free the <, >
			free(cmd_args[i + 1]); // free the filename
			++i;
		} else {
			cmd_args[j] = cmd_args[i];
			++j;
		}
	}
	cmd_args[j] = NULL;
	return cmd_args;
}

void input_redirection(char *filename)
{
	int fd;
	if((fd = open(filename, O_RDONLY)) == -1) {
		exit(1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
}

void output_redirection(char *filename) {
	int fd;
	if((fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0666)) == -1) {
		exit(1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
}


void handle_redirection(char **cmd_args)
{
	int i=0;
	for(i=0;cmd_args[i]!=NULL;i++)
	{
		if(strcmp(cmd_args[i],"<")==0)
		{
			if(cmd_args[i+1]==NULL)
				return;
			else
				input_redirection(cmd_args[i+1]);
		}

		if(strcmp(cmd_args[i],">")==0)
		{
			if(cmd_args[i+1]==NULL)
				return;
			else
				output_redirection(cmd_args[i+1]);
		}
	}
}


//Checks the redirection here
int check_redirection(char **cmd_args)
{
	int i = 0;
	for(i=0;cmd_args[i]!=NULL;i++)
	{
		if(strcmp(cmd_args[i],"<")==0||strcmp(cmd_args[i],">")==0)
			return 1;
	}
	return 0;
}

void print_parsed(char **command_args)
{
	int i=0;
	char *c;
	while((c=command_args[i++])!=NULL)
		printf("Item %d : %s\n",i,c);
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
	//printf("Command is  %s\n",cmd);
	//free(cmd);
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
	//free(command_args);
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

int execute_subcommand(char **command_args)
{
	char *cmd = command_args[0];
	char *path = command_args[1];
	char *temp;
	char *env;
	int pid,status;
	temp = malloc(BUFFER_SIZE);
	cmd = remove_trailing_space(cmd);
	//print_parsed(command_args);
	//Sort out if there is a pipe or redirection before, else do normally
		if(strcmp(cmd,"cd") == 0)
		{
			if(path!=NULL)
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
		}
		else if(strcmp(cmd,"set")==0)
		{
			if(path!=NULL)
			{
				//since putenv doesn't copy the string
				env = strdup(path);
				if(putenv(env)!=0)
					printf("Error in setting path!\n");
			}
		}
		else if(strcmp(cmd,"pwd")==0)
		{
			if (getcwd(temp,BUFFER_SIZE)!=NULL)
			{
				printf("%s\n",temp);
			}
			else
				printf("Failed fetching current working directory\n");
		}
		else if(strcmp(cmd,"exit")==0)
		{
			cleanup(command_args);
			exit(0);
		}
		else
		{//Pass these to the shell
			if((pid=fork())==0)
			{
				execvp(cmd,&command_args[0]);
				exit(0);
			}
			else if(pid>0)
			{//parent process
				waitpid(pid,&status,0);
				if(status==1)
				{
					printf("%s\n","Invalid Command!");
				}
			}
		}
	free(temp);
	return 1;		
}

char ***get_pipe_commands(char **cmd_args,char ***new_cmdargs)
{
	int subcmds=1;
	int i, j = 0;
	for (i = 0; cmd_args[i] != NULL; ++i) 
	{
		if (cmd_args[i][0] == '|') 
		{
			cmd_args[i] = NULL;
			subcmds = 1;
		} 
		else if (subcmds) 
		{
			new_cmdargs[j++] = &cmd_args[i];
			subcmds = 0;
		}
	}
	return new_cmdargs;
}

void pipe_config(int pid,int fd[2])
{
	if(pid==0)
	{
		close(fileno(stdout));
		dup(fd[1]);
		close(fd[0]);
		close(fd[1]);
	}
	else
	{
		close(fileno(stdin));
		dup(fd[0]);
		close(fd[0]);
		close(fd[1]);	
	}
}

void start_pipe_execution(char ***new_cmdargs,int count)
{
	int fd[2];
	int pid;
	if(count==0)
	{
		execute_subcommand(new_cmdargs[count]);
		return;
	}
	pipe(fd);
	pid = fork();
	pipe_config(pid,fd);
	if(pid==0)
	{
		start_pipe_execution(new_cmdargs,count-1);
		//exit(0);
	}
	else
		execute_subcommand(new_cmdargs[count]);
}


int check_background(char **cmd_args)
{
	int i=0;
	for(i=0;cmd_args[i]!=NULL;i++)
	{
		if(strcmp(cmd_args[i],"&")==0)
		{
			return 1;
		}
	}
	return 0;
}

char **remove_background(char **cmd_args)
{
	int i, j;
	for (i = 0, j = 0; cmd_args[i] != NULL; ++i) 
	{
		if (strcmp(cmd_args[i],"&") == 0)
		{
			cmd_args[i] = '\0';
			//return cmd_args;
		} 
		else 
		{
			cmd_args[j] = cmd_args[i];
			++j;
		}
	}
	cmd_args[j] = NULL;
	return cmd_args;
}

int execute_command(char **command_args)
{
	//print_parsed(command_args);
	int status;
	//Split them individually
	char ***new_cmdargs;
	int p = check_pipe(command_args);
	int r = check_redirection(command_args);
	int pid;
	int pipe_pid;
	int bg_pid;
	int b = check_background(command_args);
	//Check and see if redirection is needed
	if(r==1)
	{//setup the redirection
		if((pid=fork())==0)
		{//child process
			handle_redirection(command_args);
			command_args = remove_redirection(command_args);
			execute_command(command_args);
			exit(1);
		}
		else if(pid>0)
		{//parent process
			waitpid(pid,&status,0);
			fflush(0);
			return 1;
		}
	}
	if(p==0&&b==0)
	{//No pipes and background. Resume normal execution
		return execute_subcommand(command_args);
	}
	else if(b==1)
	{//It is a background process
		//printf("Background process\n");
		//remove the & symbol. assuming it will be in the end
		command_args = remove_background(command_args);
		if((bg_pid=fork())==0)
		{//child process
			setpgid(0, 0);
			execute_subcommand(command_args);
			exit(1);
		}
		else if(bg_pid>0)
		{//parent process
			//to signal the kernel to automatically reap the child
			signal(SIGCHLD, SIG_IGN);
			printf("%d\n",bg_pid);
		}
	}
	else if(p>0)
	{//Pipes logic goes here
		//Store all pipe commands and process one by one by a recursive
		//process
		//printf("Pipes in command %d \n",p);
		new_cmdargs = malloc((p+1)*sizeof(char **));
		new_cmdargs = get_pipe_commands(command_args,new_cmdargs);
		if((pipe_pid=fork())==0)
		{
		//child process. Should we kill? 
			start_pipe_execution(new_cmdargs,p);
			exit(0); // this is stable now
		}

		else
		//parent process
			waitpid(pipe_pid,&status,0);

		free(new_cmdargs);
		
	}
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
			//Ctrl+D comes here, terminate?
			printf("Read Error\n");
			exit(0);
			continue;
		}
		else if (strcmp(command,"\n")==0)
		{
			free(command);
			command = NULL;
			continue;			
		}
		else
		{
		command_args = parse_command(command);		
		command_status = execute_command(command_args);
		free(command);
		cleanup(command_args);
		command = NULL;
		}
	}
	return 0;
}