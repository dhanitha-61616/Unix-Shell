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
		fflush(stdout);

		/*-------------------------------------- Variables ------------------------------------------ */

		int out_red = -1;
		int out_red_count = 0;

		int in_red = -1;
		int in_red_count = 0;

		int out_append = -1;
		int out_append_count = 0;
		
		if (fgets(input, sizeof(input), stdin) == NULL)
		{
			printf("\n");
			break;
		}

		char *token = strtok(input , " \n");

		while(token != NULL && count < 99)
		{
			arr[count] = token;

			if (strcmp(token , ">") == 0)
			{
				out_red = count;
				out_red_count++;
			}

			if (strcmp(token , "<") == 0)
			{
				in_red = count;
				in_red_count++;
			}

			if (strcmp(token , ">>") == 0)
			{
				out_append = count;
				out_append_count++;
			}

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
			}
			else
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

			/*--------------------------------------- Validation ---------------------------------------------------------*/

			bool syntax_error = false;

			int red_count = out_red_count + in_red_count + out_append_count;

			/* 1. Reject repeated redirection operators */
			if (out_red_count > 1 ||
				in_red_count > 1 ||
				out_append_count > 1)
			{
				syntax_error = true;
			}

			/* 2. Reject using > and >> together */
			if (out_red != -1 && out_append != -1)
			{
				syntax_error = true;
			}

			/* 3. Validate every redirection operator */
			for (int i = 0; i < count; i++)
			{
				if (strcmp(arr[i], "<") == 0 ||
					strcmp(arr[i], ">") == 0 ||
					strcmp(arr[i], ">>") == 0)
				{
					/* Operator cannot be the last token */
					if (i == count - 1)
					{
						syntax_error = true;
						break;
					}

					/* Operator cannot be the first token */
					if (i == 0)
					{
						syntax_error = true;
						break;
					}

					/* Filename cannot be another operator */
					if (strcmp(arr[i + 1], "<") == 0 ||
						strcmp(arr[i + 1], ">") == 0 ||
						strcmp(arr[i + 1], ">>") == 0)
					{
						syntax_error = true;
						break;
					}
				}
			}

			/* 4. Validate the position of redirections */
			if (red_count > 0)
			{
				int first = count;

				if (out_red != -1 && out_red < first)
					first = out_red;

				if (in_red != -1 && in_red < first)
					first = in_red;

				if (out_append != -1 && out_append < first)
					first = out_append;

				/*
				Every redirection must be at the end:
				operator + filename = 2 tokens
				*/
				if (first != count - (red_count * 2))
				{
					syntax_error = true;
				}
			}

			/* 5. Report syntax errors */
			if (syntax_error)
			{
				fprintf(stderr, "Syntax error: invalid redirection\n");
				continue;
			}

			/*----------------------------------------------------------------------------------------------------*/


			char *input_file = NULL;
			char *output_file = NULL;
			char *append_file = NULL;

			char *args[100];
			int arg_count = 0;

			for (int i = 0; i < count; i++)
			{
				if (strcmp(arr[i], "<") == 0)
				{
					input_file = arr[i + 1];
					i++;
				}
				else if (strcmp(arr[i], ">") == 0)
				{
					output_file = arr[i + 1];
					i++;
				}else if (strcmp(arr[i] , ">>") == 0)
				{
					append_file = arr[i + 1];
					i++;
				}
				else
				{
					args[arg_count++] = arr[i];
				}
			}

			args[arg_count] = NULL;

			pid_t pid = fork();
			int status;

			if (pid == 0)
			{
			if (pid == 0)
			{
				/* Input Redirection */
				if (input_file != NULL)
				{
					int fd = open(input_file, O_RDONLY);

					if (fd == -1)
					{
						perror("open input");
						_exit(1);
					}

					if (dup2(fd, STDIN_FILENO) == -1)
					{
						perror("dup2 input");
						close(fd);
						_exit(1);
					}

					close(fd);
				}

				/* Output Redirection */
				if (output_file != NULL)
				{
					int fd = open(output_file,
								O_WRONLY | O_CREAT | O_TRUNC,
								0644);

					if (fd == -1)
					{
						perror("open output");
						_exit(1);
					}

					if (dup2(fd, STDOUT_FILENO) == -1)
					{
						perror("dup2 output");
						close(fd);
						_exit(1);
					}

					close(fd);
				}

				/* Input Append */
				if(out_append != -1)
				{
					int fd = open(append_file,
								O_WRONLY | O_CREAT | O_APPEND,
								0644);

					if (fd == -1)
					{
						perror("open output");
						_exit(1);
					}

					if (dup2(fd, STDOUT_FILENO) == -1)
					{
						perror("dup2 output");
						close(fd);
						_exit(1);
					}

					close(fd);
				}

				execvp(args[0], args);

				perror(args[0]);
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
