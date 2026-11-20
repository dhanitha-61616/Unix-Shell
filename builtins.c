#include "shell.h"

int handle_builtin(Command *cmd)
{
    /* exit */
    if (strcmp(cmd->args[0], "exit") == 0)
    {
        exit(0);
    }

    /* cd */
    if (strcmp(cmd->args[0], "cd") == 0)
    {
        char *directory;

        if (cmd->arg_count == 1)
        {
            directory = getenv("HOME");
        }
        else
        {
            directory = cmd->args[1];
        }

        if (directory == NULL)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }

        if (chdir(directory) == -1)
        {
            perror("cd");
        }

        return 1;
    }

    /* Not a built-in command */
    return 0;
}