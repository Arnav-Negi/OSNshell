#include "signal_handling.h"
#include "../helper/prompt.h"

int newbg(int pid, char *proccess_name, char *command)
{
    if (pid == 0)
        return 0;

    int i = 0;
    while (i < 1000 && bgpid[i] != NULL)
        i++;
    if (i == 1000)
    {
        printf(KRED "Process limit reached.\n" RESET);
        exit(1);
    }
    printf("[%d] %d\n", i + 1, pid);
    bgpid[i] = (procinfo *)malloc(sizeof(procinfo));
    bgpid[i]->pid = pid;
    bgpid[i]->status = 1;
    bgpid[i]->jobno = i + 1;
    bgpid[i]->procname = (char *)malloc(strlen(proccess_name) + 1);
    bgpid[i]->command = (char *)malloc(strlen(command) + 1);
    bgpid[i]->pgpid = pid;
    strcpy(bgpid[i]->procname, proccess_name);
    strcpy(bgpid[i]->command, command);

    return 0;
}

void ctrlZ_handler(int signum)
{
    // Stop current process, send to bg.
    printf("hello");
    if (fgpid.pid == -1)
        return;

    tcsetpgrp(STDIN_FILENO, getpgid(getpid()));
    tcsetpgrp(STDOUT_FILENO, getpgid(getpid()));
    kill(fgpid.pid, SIGTSTP);
    newbg(fgpid.pid, fgpid.procname, fgpid.command);
    fgpid.pid = -1;
    free(fgpid.command), free(fgpid.procname);
    return;
}

void ctrlC_handler(int signum)
{
}

void ctrlD_handler(int signum)
{
}

void rembg(int signum)
{
    int status;
    int pid = waitpid(-1, &status, WNOHANG);
    if (pid == lastpipepid)
    {
        pipeflag = 0;
        lastpipepid = -1;
    }
    int i = 0;
    if (pid == 0)
        return;
    while (i < 1000)
    {
        if (bgpid[i] != NULL && bgpid[i]->pid == pid)
            break;
        i++;
    }
    if (i == 1000)
    {
        return;
    }

    if (pid > 0)
    {
        if (WIFSIGNALED(status))
        {
            printf(KRED "\n%s with pid %d exited abnormally\n" RESET, bgpid[i]->procname, pid);
        }
        if (WIFEXITED(status))
        {
            printf(KGRN "\n%s with pid %d exited normally\n" RESET, bgpid[i]->procname, pid);
        }
        if (isprompt)
        {
            long int temp = prevcmd_time;
            prevcmd_time = -1;
            prompt();
            prevcmd_time = temp;
            isprompt = 0;
        }
    }

    free(bgpid[i]->procname);
    free(bgpid[i]->command);
    free(bgpid[i]);
    bgpid[i] = NULL;
}