#include "signal_handling.h"
#include "../helper/prompt.h"

int newbg(int pid, char *proccess_name, char *command, int status)
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
    bgpid[i]->status = status;
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
    if (fgpid.pid == -1)
        return;

    tcsetpgrp(STDIN_FILENO, getpgid(getpid()));
    tcsetpgrp(STDOUT_FILENO, getpgid(getpid()));
    // kill(fgpid.pid, SIGTSTP);
    newbg(fgpid.pid, fgpid.procname, fgpid.command, 0);
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
    int status, notbg = 0;
    int pid = waitpid(-1, &status, WUNTRACED);

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
        notbg = 1;
    }

    if (pid > 0)
    {
        if (WIFSTOPPED(status) && notbg)
        {
            switch (WSTOPSIG(status))
            {
            case SIGTSTP:
                ctrlZ_handler(1);
                break;
            case SIGINT:
                ctrlC_handler(1);
                break;
            default:
                printf(KRED "\n%s with pid %d exited abnormally\n" RESET, bgpid[i]->procname, pid);
                break;
            }
        }
        else if (!notbg && WIFEXITED(status))
        {
            if (WEXITSTATUS(status) == 0)
                printf(KGRN "\n%s with pid %d exited normally\n" RESET, bgpid[i]->procname, pid);
            else
                printf(KRED "\n%s with pid %d exited abnormally\n" RESET, bgpid[i]->procname, pid);
        }

        if (isrunning == 1 && notbg)
        {
            long int temp = prevcmd_time;
            prevcmd_time = -1;
            prompt();
            prevcmd_time = temp;
            isrunning = 0;
        }
    }
    if (pid == pipeflag) pipesleft--;
    if (!notbg)
    {
        free(bgpid[i]->procname);
        free(bgpid[i]->command);
        free(bgpid[i]);
        bgpid[i] = NULL;
    }
}

void DoNothing(int signum)
{

    printf("\n");
    prompt();
}