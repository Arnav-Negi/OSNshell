#include "cd.h"
#include "../included.h"

extern char *prevcd;

void swap(char *s1, char *s2)
{
    char *temp;
    temp = s2;
    s2 = s1;
    s1 = temp;
}

void cd(char **args, sysinfo *currsys)
{
    int status;
    char *tempcurrdir, *tempprevdir;
    tempcurrdir = (char *)malloc(strlen(currsys->curr_dir) + 5);
    tempprevdir = (char *)malloc(strlen(prevcd) + 5);
    strcpy(tempcurrdir, currsys->curr_dir);
    strcpy(tempprevdir, prevcd);


    if (args[1] != NULL && args[2] != NULL)
    {
        printf("cd: Too many arguments.");
        
    }

    else if (args[1] == NULL || strcmp(args[1], "~") == 0)
    {
        swap(currsys->curr_dir, prevcd);
        strcpy(currsys->curr_dir, currsys->home_dir);
        status = chdir(currsys->home_dir);
    }

    else if (strcmp(args[1], "-") == 0)
    {
        swap(currsys->curr_dir, prevcd);
        status = chdir(currsys->curr_dir);
        printf("%s", currsys->curr_dir);
    }

    else if (args[1][0] == '/')
    {
        swap(currsys->curr_dir, prevcd);
        status = chdir(args[1]);
        if (strlen(args[1]) > strlen(currsys->curr_dir))
        {
            if (realloc(currsys->curr_dir, strlen(args[1]) + 25) == NULL) {
                printf("No memory to change directories.");
                status = -1;
            }
        }
        strcpy(currsys->curr_dir, args[1]);
        
    }

    else if (args[1][0] != '/')
    {
        free(prevcd);
        prevcd = currsys->curr_dir;

        currsys->curr_dir = (char *) malloc(strlen(args[1]) + strlen(prevcd) + 25);
        strcpy(currsys->curr_dir, prevcd);
        strcat(currsys->curr_dir, "/");
        strcat(currsys->curr_dir, args[1]);

        status = chdir(currsys->curr_dir);
    }

    if (status != 0)
    {
        perror("Error while changing directories");
        free(currsys->curr_dir);
        free(prevcd);
        currsys->curr_dir = tempcurrdir;
        prevcd = tempprevdir;
    }
    else {
        free(tempcurrdir);
        free(tempprevdir);
    }
}