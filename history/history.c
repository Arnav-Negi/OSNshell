#include "history.h"
#include "../included.h"

void print_history(sysinfo *currsys)
{
    char **commandlist = malloc(sizeof(char *) * (DEF_HIST_SIZE + 3));
    int l = getcommands(commandlist, currsys);

    for (int i = 0; i<l; i++)
    {
        if (i >= l - DEF_HIST_OUT)
            printf("%s", commandlist[i]);
        free(commandlist[i]);
    }
    free(commandlist);
    return;
}

int getcommands(char **commandlist, sysinfo *currsys)
{
    char *path = malloc(strlen(currsys->home_dir) + 25);
    strcpy(path, currsys->home_dir);
    strcat(path, "/history/.history.txt");

    FILE *readfile = fopen(path, "r");
    long unsigned int n = 0;
    int i = 0;
    commandlist[0] = NULL;
    while (getline(&commandlist[i], &n, readfile) > 0)
    {
        i++;
        n = 0;
        commandlist[i] = NULL;
    }
    free(path);
    fclose(readfile);
    return i;
}

int addhistory(int argc, char **argv, sysinfo *currsys)
{
    char **commandlist = malloc(sizeof(char *) * (DEF_HIST_SIZE + 3));
    int lines = getcommands(commandlist, currsys);

    char *path = malloc(strlen(currsys->home_dir) + 25);
    strcpy(path, currsys->home_dir);
    strcat(path, "/history/.history.txt");

    int buffersize = 0;
    for (int i = 0; i < argc; i++)
    {
        buffersize += strlen(argv[i]) + 1;
    }

    char *newcmd = malloc(buffersize + 10);
    strcpy(newcmd, argv[0]);
    for (int i = 1; i < argc; i++)
    {
        strcat(newcmd, " ");
        strcat(newcmd, argv[i]);
    }
    strcat(newcmd, "\n");
    if (lines > 0 && strcmp(newcmd, commandlist[lines - 1]) == 0)
    {
        for (int i = 0; i < lines; i++)
        {
            free(commandlist[i]);
        }
        free(commandlist), free(path), free(newcmd);
        return 1;
    }

    if (lines >= DEF_HIST_SIZE)
    {
        // replace top line.
        FILE *writefile = fopen(path, "w");
        int i = lines - DEF_HIST_SIZE + 1;
        while (i < lines)
        {
            fprintf(writefile, "%s", commandlist[i]);
            free(commandlist[i]);
            i++;
        }
        fprintf(writefile, "%s", newcmd);
        fflush(writefile);
        fclose(writefile);
    }
    else
    {
        // Add on to file on top.
        FILE *appendfile = fopen(path, "a");
        fprintf(appendfile, "%s", newcmd);
        for (int i = 0; i < lines; i++)
        {
            free(commandlist[i]);
        }
        fflush(appendfile);
        fclose(appendfile);
    }

    free(commandlist), free(path), free(newcmd);
    return 1;
}