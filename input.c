#include "shell.h"

int read_input(char *input, size_t size)
{
    if (fgets(input, size, stdin) == NULL)
    {
        return 0;
    }

    return 1;
}