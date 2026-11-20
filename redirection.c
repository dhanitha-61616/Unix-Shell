#include "shell.h"

int apply_redirections(Command *cmd)
{
    int fd;

    /* Input redirection: < */
    if (cmd->input_file != NULL)
    {
        fd = open(cmd->input_file, O_RDONLY);

        if (fd == -1)
        {
            perror(cmd->input_file);
            return -1;
        }

        if (dup2(fd, STDIN_FILENO) == -1)
        {
            perror("dup2");
            close(fd);
            return -1;
        }

        close(fd);
    }

    /* Output redirection: > */
    if (cmd->output_file != NULL)
    {
        fd = open(cmd->output_file,
                  O_WRONLY | O_CREAT | O_TRUNC,
                  0644);

        if (fd == -1)
        {
            perror(cmd->output_file);
            return -1;
        }

        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            close(fd);
            return -1;
        }

        close(fd);
    }

    /* Append redirection: >> */
    if (cmd->append_file != NULL)
    {
        fd = open(cmd->append_file,
                  O_WRONLY | O_CREAT | O_APPEND,
                  0644);

        if (fd == -1)
        {
            perror(cmd->append_file);
            return -1;
        }

        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            close(fd);
            return -1;
        }

        close(fd);
    }

    return 0;
}