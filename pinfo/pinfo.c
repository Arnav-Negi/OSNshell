#include "../included.h"
#include "../inputs.h"

int pinfo(int pid, sysinfo *currsys)
{
    long long int vmsize;
    char state = ' ';
    char procname[200], execpath[500], *relexec;
    int pgpid, fgpid;

    if (pid == -1)
        pid = getpid();

    char pathname[100];
    sprintf(pathname, "/proc/%d/stat", pid);
    FILE *statfile = fopen(pathname, "r");
    if (statfile == NULL)
    {
        printf(KRED"Process with given pid not found\n"RESET);
        return 1;
    }
    fscanf(statfile, "%d", &pid);
    fscanf(statfile, "%s", procname);
    fscanf(statfile, "%c", &state);
    fscanf(statfile, "%c", &state);
    fscanf(statfile, "%d", &pgpid);
    fscanf(statfile, "%d", &pgpid);
    fscanf(statfile, "%d", &fgpid);
    fscanf(statfile, "%d", &fgpid);
    fscanf(statfile, "%d", &fgpid);
    for (int i = 0; i<15; i++)
    {
        fscanf(statfile, "%lld", &vmsize);
    }
    fclose(statfile);

    sprintf(pathname, "/proc/%d/exe", pid);
    readlink(pathname, execpath, 500);
    relexec = convert_to_tilde(execpath, currsys);


    printf("pid : %d\n", pid);
    printf("process Status : %c", state);
    if (pgpid == fgpid) printf("+");
    printf("\nmemory : %lld\n", vmsize);
    printf("executable Path : %s\n", relexec);

    free(relexec);
    return 0;
}