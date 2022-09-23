#include "fg.h"

int fg(int argc, char **args, procinfo *arr[])
{
    if (argc != 2)
    {
        printf(KRED "Syntax error:" RESET " correct syntax is fg <job number>.\n");
        return 1;
    }
    int status;
    int jobno = strtol(args[1], NULL, 10) - 1;
    if (arr[jobno] == NULL)
    {
        printf(KRED "Error:" RESET " given job number doesn't exist.\n");
        return 1;
    }

    int pgpid = arr[jobno]->pgpid, pid = arr[jobno]->pid;
    if (kill(pid, SIGCONT) == -1) {
        perror(KRED"error"RESET);
        return 1;
    }

    fgpid.procname = arr[jobno]->procname;
    fgpid.pgpid = arr[jobno]->pgpid;
    fgpid.status = 1;
    fgpid.pid = arr[jobno]->pid;
    fgpid.command = arr[jobno]->command;
    fgpid.jobno = jobno;

    arr[jobno] = NULL;

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    tcsetpgrp(STDIN_FILENO, pgpid);
    tcsetpgrp(STDOUT_FILENO, pgpid);

    waitpid(pid, &status, WUNTRACED);

    tcsetpgrp(STDIN_FILENO, getpgrp());
    tcsetpgrp(STDOUT_FILENO, getpgrp());
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    return 0;
}