#include "bg.h"

int bg(int argc, char **args, procinfo *array[])
{
    if (argc != 2)
    {
        printf(KRED "Syntax error:" RESET " correct syntax is bg <job number>.\n");
        return 1;
    }

    int jobno = strtol(args[1], NULL, 10) - 1;
    if (array[jobno] == NULL)
    {
        printf(KRED "Error:" RESET " given job number doesn't exist.\n");
        return 1;
    }

    int pid = array[jobno]->pid;

    if (kill(pid, SIGCONT) == -1) {
        perror(KRED"error"RESET);
        return 1;
    }
    return 0;
}