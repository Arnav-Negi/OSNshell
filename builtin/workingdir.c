#include "workingdir.h"

char *pwd()
{
    int BUF = DEF_BUF_SIZE;

    char *path = (char *)malloc(BUF);
    while (getcwd(path, BUF) == NULL)
    {
        BUF += BUF_SZ_INC;
        path = (char *)realloc(path, BUF);
    }

    return path;
}