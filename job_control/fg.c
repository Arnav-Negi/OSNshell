#include "fg.h"

int fg(int argc, char **args)
{
    if (argc != 2)
    {
        printf(KRED "Syntax error:" RESET " correct syntax is fg <job number>.\n");
        return 1;
    }

    int jobno = strtol(args[1], NULL, 10) - 1;
    if (bgpid[jobno] == NULL)
    {
        printf(KRED "Error:" RESET " given job number doesn't exist.\n");
        return 1;
    }

    int pgpid = bgpid[jobno]->pgpid, pid = bgpid[jobno]->pid;

    fgpid.procname = bgpid[jobno]->procname;
    fgpid.pgpid = bgpid[jobno]->pgpid;
    fgpid.status = 1;
    fgpid.pid = bgpid[jobno]->pid;
    fgpid.command = bgpid[jobno]->command;
    fgpid.jobno = jobno;

    free(bgpid[jobno]);
    bgpid[jobno] = NULL;

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    tcsetpgrp(STDIN_FILENO, pgpid);
    tcsetpgrp(STDOUT_FILENO, pgpid);

    if (kill(pid, SIGCONT) == -1)
    {
        perror(KRED "error" RESET);
        return 1;
    }
    isrunning = 1;
    while (isrunning)
    {
    }

    tcsetpgrp(STDIN_FILENO, getpgrp());
    tcsetpgrp(STDOUT_FILENO, getpgrp());
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    return 0;
}