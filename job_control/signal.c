#include "signal.h"

int sig(procinfo *arr[], int argc, char **args)
{
    int pid, signum, jno;

    if (argc != 3) {
        printf(KRED"Syntax error: sig <pid> <signal>\n"RESET);
        return 1;
    }
    jno = strtol(args[1], NULL, 10) - 1;
    signum = strtol(args[2], NULL, 10);
    if (arr[jno] == NULL) {
        printf(KRED"Error: "RESET "Job number doesn't exist.\n");
        return 1;
    }

    pid = arr[jno]->pid;

    if (kill(pid, signum) < 0) {
        perror(KRED"Sig error"RESET);
        return 1;
    }

    return 0;
}