#include "../included.h"
#include "../inputs.h"

int pinfo(int pid, procinfo **procarr, sysinfo *currsys)
{
    int isbg = 0;
    long long int vmsize;
    char state = ' ';
    char procname[200], execpath[500], *relexec;

    if (pid == -1)
        pid = getpid();

    for (int i = 0; i < 1000; i++)
    {
        if (procarr[i] != NULL && procarr[i]->pid == pid)
        {
            isbg = 1;
            break;
        }
    }

    char pathname[100];
    sprintf(pathname, "/proc/%d/stat", pid);
    FILE *statfile = fopen(pathname, "r");
    if (statfile == NULL)
    {
        printf("Process with given pid not found\n");
        return 1;
    }
    fscanf(statfile, "%d", &pid);
    fscanf(statfile, "%s", procname);
    fscanf(statfile, "%c", &state);
    fscanf(statfile, "%c", &state);
    for (int i = 0; i<20; i++)
    {
        fscanf(statfile, "%lld", &vmsize);
    }
    fclose(statfile);

    sprintf(pathname, "/proc/%d/exe", pid);
    readlink(pathname, execpath, 500);
    relexec = convert_to_tilde(execpath, currsys);


    printf("pid : %d\n", pid);
    printf("process Status : %c", state);
    if (!isbg) printf("+");
    printf("\nmemory : %lld\n", vmsize);
    printf("executable Path : %s\n", relexec);

    free(relexec);
    return 0;
}