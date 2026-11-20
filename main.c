
#include "shell.h"

int main(void)
{
    char input[MAX_INPUT];

    while (1)
    {
        /* Display username and current directory */
        char *username = getenv("USER");
        char cwd[1024];

        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("getcwd");
            strcpy(cwd, "?");
        }

        printf(BLUE "%s" RESET ":" BLUE "%s" RESET "$ ",
               username ? username : "user",
               cwd);

        fflush(stdout);

        /* Read input */
        if (!read_input(input, sizeof(input)))
        {
            printf("\n");
            break;
        }
        /* Parse command */
        Command cmd;

        int result = parse_command(input, &cmd);

        if (result == -1)
        {
            continue;
        }

        if (result == 0)
        {
            continue;
        }

        /* Handle built-in commands */
        if (handle_builtin(&cmd))
        {
            continue;
        }

        /* Execute external command */
        execute_command(&cmd);

        
    }

    return 0;
}
