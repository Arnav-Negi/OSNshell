#include "cd.h"
#include "pwd.h"
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
    int status = 0;


    if (args[1] != NULL && args[2] != NULL)
    {
        printf("cd: Too many arguments.");
    }

    else if (args[1] == NULL || strcmp(args[1], "~") == 0)
    {
        status = chdir(currsys->home_dir);
    }

    else if (strcmp(args[1], "-") == 0)
    {
        status = chdir(prevcd);
        printf("%s", prevcd);
    }

    else
    {
        status = chdir(args[1]);        
    }

    if (status != 0)
    {
        perror("Error while changing directories");
    }
    else {
        prevcd = currsys->curr_dir;
        currsys->curr_dir = pwd();
    }
}