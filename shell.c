#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>

#define BLUE "\033[34m"
#define RESET "\033[0m"

int main(void)
{
	while (true)
	{

		/* Environment Variables */
		char *USER = getenv("USER");

		if (USER == NULL)
		{
			USER = "user";
		}

		printf(BLUE "%s " RESET ,  USER);

		char cwd[4096];

		if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
			printf(BLUE "%s" RESET , cwd);
			setenv("PWD", cwd, 1);
		}
		else
		{
			perror("getcwd");
		}

		/*------------------------------------------------------------------------------------------------*/
		/* ------------------------------------Inputs--------------------------------------- */
		char input[1000];
		char *arr[100];
		int count = 0 ;
		printf("$ ");
		fflush(stdin);
		
		if (fgets(input, sizeof(input), stdin) == NULL)
		{
			printf("\n");
			break;
		}

		char *token = strtok(input , " \n");

		while(token != NULL && count < 99)
		{
			arr[count] = token;
			count++;

			token = strtok(NULL , " \n");
		}

		arr[count] = NULL;

		/* ----------------------------------------------------------------------------------------------------*/
		/*------------------------------------ Command Process -------------------------------------- */
		
		if (arr[0] == NULL)
		{
			continue;
		}else if (strcmp(arr[0] , "exit") == 0)
		{
			return 0;
		}else if(strcmp(arr[0] , "cd") == 0)
		{

			char *destination;

			if (arr[1] == NULL)
			{
				destination = getenv("HOME");
			}
			else
			{
				destination = arr[1];
			}

			if (destination == NULL)
			{
				fprintf(stderr, "cd: HOME not set\n");
			}
			else if (chdir(destination) == -1)
			{
				perror("cd");
			}else
			{

				if (getcwd(cwd , sizeof(cwd)) != NULL)
				{
					if (setenv("PWD", cwd, 1) == -1)
					{
						perror("setenv");
					}
				}else{
					perror("getcwd");
				}
			}

		}else
		{
			pid_t pid = fork();
			int status;

			if (pid == 0)
			{
				int task = execvp(arr[0] , arr );

				if (task == -1)
				{
					perror(arr[0]);
					_exit(127);
				}
			}
			else if (pid > 0)
			{
				if (waitpid(pid, &status, 0) == -1)
				{
					perror("waitpid");
				}
			}
			else
			{
				perror("fork");
			}
		}

		
	}

	return 0;
}
