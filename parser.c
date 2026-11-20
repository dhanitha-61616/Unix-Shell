#include "shell.h"

int parse_command(char *input, Command *cmd)
{
    memset(cmd, 0, sizeof(*cmd));

    cmd->in_red = -1;
    cmd->out_red = -1;
    cmd->out_append = -1;

    /* Tokenize input */
    char *token = strtok(input, " \t\n");

    while (token != NULL)
    {
        if (cmd->count >= MAX_ARGS - 1)
        {
            fprintf(stderr, "Error: Too many arguments\n");
            return -1;
        }

        cmd->tokens[cmd->count++] = token;
        token = strtok(NULL, " \t\n");
    }

    cmd->tokens[cmd->count] = NULL;

    /* Empty input */
    if (cmd->count == 0)
    {
        return 0;
    }

    /* Identify redirection operators */
    for (int i = 0; i < cmd->count; i++)
    {
        if (strcmp(cmd->tokens[i], "<") == 0)
        {
            if (cmd->in_red != -1)
            {
                fprintf(stderr, "Error: Multiple input redirections\n");
                return -1;
            }

            cmd->in_red = i;
        }

        else if (strcmp(cmd->tokens[i], ">") == 0)
        {
            if (cmd->out_red != -1)
            {
                fprintf(stderr, "Error: Multiple output redirections\n");
                return -1;
            }

            cmd->out_red = i;
        }

        else if (strcmp(cmd->tokens[i], ">>") == 0)
        {
            if (cmd->out_append != -1)
            {
                fprintf(stderr, "Error: Multiple append redirections\n");
                return -1;
            }

            cmd->out_append = i;
        }
    }

    /* Cannot use > and >> together */
    if (cmd->out_red != -1 && cmd->out_append != -1)
    {
        fprintf(stderr, "Error: Cannot use > and >> together\n");
        return -1;
    }

    /* Validate redirection operators */
    for (int i = 0; i < cmd->count; i++)
    {
        if (strcmp(cmd->tokens[i], "<") == 0 ||
            strcmp(cmd->tokens[i], ">") == 0 ||
            strcmp(cmd->tokens[i], ">>") == 0)
        {
            /* Operator cannot be first or last */
            if (i == 0 || i == cmd->count - 1)
            {
                fprintf(stderr, "Error: Invalid redirection syntax\n");
                return -1;
            }

            /* Another operator cannot be the filename */
            if (strcmp(cmd->tokens[i + 1], "<") == 0 ||
                strcmp(cmd->tokens[i + 1], ">") == 0 ||
                strcmp(cmd->tokens[i + 1], ">>") == 0)
            {
                fprintf(stderr, "Error: Missing redirection filename\n");
                return -1;
            }
        }
    }

    /* Count redirection operators */
    int red_count = 0;

    if (cmd->in_red != -1)
        red_count++;

    if (cmd->out_red != -1)
        red_count++;

    if (cmd->out_append != -1)
        red_count++;

    /* Redirections must be at the end */
    if (red_count > 0)
    {
        int first_red = cmd->count;

        if (cmd->in_red != -1 && cmd->in_red < first_red)
            first_red = cmd->in_red;

        if (cmd->out_red != -1 && cmd->out_red < first_red)
            first_red = cmd->out_red;

        if (cmd->out_append != -1 && cmd->out_append < first_red)
            first_red = cmd->out_append;

        if (first_red != cmd->count - (red_count * 2))
        {
            fprintf(stderr, "Error: Redirections must be at the end\n");
            return -1;
        }
    }

    /* Extract filenames */
    if (cmd->in_red != -1)
    {
        cmd->input_file = cmd->tokens[cmd->in_red + 1];
    }

    if (cmd->out_red != -1)
    {
        cmd->output_file = cmd->tokens[cmd->out_red + 1];
    }

    if (cmd->out_append != -1)
    {
        cmd->append_file = cmd->tokens[cmd->out_append + 1];
    }

    /* Build clean argument list */
    for (int i = 0; i < cmd->count; i++)
    {
        /* Skip redirection operators and their filenames */
        if (strcmp(cmd->tokens[i], "<") == 0 ||
            strcmp(cmd->tokens[i], ">") == 0 ||
            strcmp(cmd->tokens[i], ">>") == 0)
        {
            i++; 
            continue;
        }

        cmd->args[cmd->arg_count++] = cmd->tokens[i];
    }

    cmd->args[cmd->arg_count] = NULL;

    return 1;
}