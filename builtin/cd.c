#include "cd.h"
#include "workingdir.h"
#include "../included.h"
#include "../helper/inputs.h"

extern char *prevcd;

int cd(char **args, sysinfo *currsys)
{
    int status = 0;

    if (args[1] != NULL && args[2] != NULL)
    {
        printf(KRED "cd: Too many arguments.\n" RESET);
    }

    else if (args[1] == NULL || strcmp(args[1], "~") == 0 || strcmp(args[1], "~/") == 0)
    {
        status = chdir(currsys->home_dir);
    }

    else if (args[1][0] == '~' && args[1][2] != '\0')
    {
        args[1] = convert_from_tilde(&args[1][2], currsys);
        status = chdir(args[1]);
        
        free(args[1]);
    }

    else if (strcmp(args[1], "-") == 0)
    {
        status = chdir(prevcd);
        printf("%s\n", prevcd);
    }

    else
    {
        status = chdir(args[1]);
    }

    if (status != 0)
    {
        perror(KRED "Error while changing directories" RESET);
    }
    else
    {
        free(prevcd);
        prevcd = currsys->curr_dir;
        currsys->curr_dir = pwd();
    }

    return status;
}