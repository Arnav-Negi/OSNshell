#include "../included.h"
#include "../helper/inputs.h"
#include "../builtin/cd.h"
#include "../builtin/workingdir.h"

char *currdir;
int lencurrdir;
struct stat *statbuf;

void printdir(char *dirpath, int flag, char *file)
{
    struct dirent **namelist;
    int len = scandir(dirpath, &namelist, NULL, alphasort);
    for (int i = 0; i < len; i++)
    {
        if (strcmp(namelist[i]->d_name, ".") == 0)
        {
            free(namelist[i]);
            continue;
        }
        if (strcmp(namelist[i]->d_name, "..") == 0)
        {
            free(namelist[i]);
            continue;
        }
        strcat(currdir, "/");
        strcat(currdir, namelist[i]->d_name);
        lencurrdir += strlen(namelist[i]->d_name) + 1;
        if (stat(currdir, statbuf) == -1)
        {
            while (currdir[lencurrdir - 1] != '/')
            {
                currdir[lencurrdir - 1] = '\0';
                lencurrdir--;
            }
            currdir[lencurrdir - 1] = '\0';
            lencurrdir--;
            free(namelist[i]);
            continue;
        }

        if (file != NULL && strcmp(file, namelist[i]->d_name) == 0)
        {
            if ((statbuf->st_mode & S_IFMT) == S_IFDIR)
            {
                if (flag & 1)
                {
                    printf("%s\n", currdir);
                }
            }
            else
            {
                if (flag & 2)
                {
                    printf("%s\n", currdir);
                }
            }
        }
        else if (file == NULL)
        {
            if ((statbuf->st_mode & S_IFMT) == S_IFDIR)
            {
                if (flag & 1)
                {
                    printf("%s\n", currdir);
                }
            }
            else
            {
                if (flag & 2)
                {
                    printf("%s\n", currdir);
                }
            }
        }
        printdir(currdir, flag, file);

        while (currdir[lencurrdir - 1] != '/')
        {
            currdir[lencurrdir - 1] = '\0';
            lencurrdir--;
        }
        currdir[lencurrdir - 1] = '\0';
        lencurrdir--;
        free(namelist[i]);
    }
    if (len > -1)
        free(namelist);
}

int discover(int argc, char **argv, sysinfo *currsys)
{
    statbuf = malloc(sizeof(struct stat));
    int flag = 0, i, prevleak = 0;
    DIR *tempdir;
    char *filename = NULL, *targetdir = NULL;

    currdir = malloc(sizeof(char) * 500);
    currdir[0] = '.', currdir[1] = '\0';

    // -d flag is 01, -f flag is 10, default is 11 (both files and directories).
    i = 1;
    while (argv[i] != NULL)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            flag |= 1;
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            flag |= 2;
        }
        else if (argv[i][0] == '-')
        {
            printf(KRED "discover error" RESET ": invalid option %s\n", argv[i]);
        }
        else if (argv[i][0] == '"')
        {
            if (argv[i][strlen(argv[i]) - 1] != '"')
            {
                printf(KRED "discover error" RESET ": invalid filename, should be in quotations\n");
                if (prevleak)
                    free(targetdir);
                return 1;
            }
            filename = &argv[i][1];
            filename[strlen(filename) - 1] = '\0';
        }
        else
        {
            if (targetdir != NULL)
            {
                printf(KRED "discover error" RESET ": multiple target directories given\n");
                return 1;
            }
            targetdir = argv[i];
            if (strcmp(targetdir, "~") == 0 || strcmp(targetdir, "~/") == 0)
            {
                targetdir = currsys->home_dir;
            }
            if (targetdir[0] == '~')
            {
                targetdir = convert_from_tilde(&targetdir[2], currsys);
                prevleak = 1;
            }
            if (targetdir[strlen(targetdir) - 1] == '/')
            {
                targetdir[strlen(targetdir) - 1] = '\0';
            }
            if (strcmp(targetdir, ".") == 0)
                targetdir = NULL;
        }

        i++;
    }

    if (flag == 0)
        flag = 3;
    if (targetdir != NULL)
    {
        if ((tempdir = opendir(targetdir)) == NULL)
        {
            printf(KRED "Target directory doesn't exist\n" RESET);
            free(currdir), free(statbuf);
            return 1;
        }
        if (targetdir[0] == '/')
        {
            strcpy(currdir, targetdir);
        }
        else
        {
            strcat(currdir, "/");
            strcat(currdir, targetdir);
        }
        free(tempdir);
    }
    lencurrdir = strlen(currdir);
    if ((flag & 1) && filename == NULL)
        printf("%s\n", currdir);
    printdir(currdir, flag, filename);

    free(currdir), free(statbuf);
    if (prevleak)
        free(targetdir);
    return 0;
}