#include "shell.h"

void execute_command(Command *cmd)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    /* Child process */
    if (pid == 0)
    {
        if (apply_redirections(cmd) == -1)
        {
            exit(EXIT_FAILURE);
        }

        execvp(cmd->args[0], cmd->args);

        /* Reached only if execvp fails */
        perror(cmd->args[0]);
        exit(EXIT_FAILURE);
    }

    /* Parent process */
    else
    {
        int status;

        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid");
        }
    }
}

void execute_pipeline(Pipeline *pipeline)
{
    int fd[2];

    if (pipe(fd) == -1)
    {
        perror("pipe");
        return;
    }
    else
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork");
            return;
        }
        else if (pid > 0)
        {
            pid_t pid2 = fork();

            if (pid2 == -1)
            {
                perror("fork");

                close(fd[0]);
                close(fd[1]);

                int status;

                if (waitpid(pid, &status, 0) == -1)
                {
                    perror("waitpid");
                }

                return;
            }
            else if (pid2 == 0)
            {
                if (dup2(fd[0] , STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    _exit(EXIT_FAILURE);
                }
                else 
                {
                    close(fd[0]);
                    close(fd[1]);

                    execvp(pipeline->commands[1].args[0], pipeline->commands[1].args);

                    /* Reached only if execvp fails */
                    perror(pipeline->commands[1].args[0]);
                    _exit(EXIT_FAILURE);
                }
            }

            close(fd[0]);
            close(fd[1]);

            int status;

            if (waitpid(pid, &status, 0) == -1)
            {
                perror("waitpid");
            }

            if (waitpid(pid2, &status, 0) == -1)
            {
                perror("waitpid");
            }
        }
        else if (pid == 0)
        {
            if (dup2(fd[1] , STDOUT_FILENO) == -1)
            {
                perror("dup2");
                _exit(EXIT_FAILURE);
            }
            else 
            {
                close(fd[0]);
                close(fd[1]);

                execvp(pipeline->commands[0].args[0], pipeline->commands[0].args);

                    /* Reached only if execvp fails */
                    perror(pipeline->commands[0].args[0]);
                    _exit(EXIT_FAILURE);
            }
        }
    }
}