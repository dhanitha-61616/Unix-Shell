#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BLUE "\033[34m"
#define RESET "\033[0m"

#define MAX_INPUT 1000
#define MAX_ARGS 100

typedef struct
{
    char *tokens[MAX_ARGS];
    int count;

    char *args[MAX_ARGS];
    int arg_count;

    char *input_file;
    char *output_file;
    char *append_file;

    int in_red;
    int out_red;
    int out_append;

} Command;

#define MAX_COMMANDS 20

typedef struct
{
    Command commands[MAX_COMMANDS];
    int count;

} Pipeline;

/* Input */
int read_input(char *input, size_t size);

/* Parsing */
int parse_command(char *input, Command *cmd);
int parse_pipeline(char *input, Pipeline *pipeline);

/* Built-ins */
int handle_builtin(Command *cmd);

/* Redirection */
int apply_redirections(Command *cmd);

/* Execution */
void execute_command(Command *cmd);
void execute_pipeline(Pipeline *pipeline);

#endif