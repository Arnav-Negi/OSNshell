#include "echo.h"
#include "included.h"

void echo(char **args)
{
    int i = 1;
    while (args[i] != NULL)
    {
        printf("%s ", args[i++]);
    }
    printf("\n");
}