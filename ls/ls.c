#include "../included.h"
#include "ls.h"
struct stat *statbufls;
char *months[] = {"January  ", "February ", "March    ", "April    ", "May      ", "June     ", "July     ", "August   ", "September", "October  ", "November ", "December "};

int filter(const struct dirent *a)
{
    return a->d_name[0] != '.';
}

void printentry(char *filepath, char *filename)
{
    struct passwd *temppwd;
    struct group *tempgrp;
    stat(filepath, statbufls);
    tempgrp = getgrgid(statbufls->st_gid);
    temppwd = getpwuid(statbufls->st_uid);
    if ((statbufls->st_mode & S_IFMT) == S_IFDIR)
        printf("d");
    else
        printf("-");
    if (statbufls->st_mode & S_IRUSR)
        printf("r");
    else
        printf("-");
    if (statbufls->st_mode & S_IWUSR)
        printf("w");
    else
        printf("-");
    if (statbufls->st_mode & S_IXUSR)
        printf("x");
    else
        printf("-");
    if (statbufls->st_mode & S_IRGRP)
        printf("r");
    else
        printf("-");
    if (statbufls->st_mode & S_IWGRP)
        printf("w");
    else
        printf("-");
    if (statbufls->st_mode & S_IXGRP)
        printf("x");
    else
        printf("-");
    if (statbufls->st_mode & S_IROTH)
        printf("r");
    else
        printf("-");
    if (statbufls->st_mode & S_IWOTH)
        printf("w");
    else
        printf("-");
    if (statbufls->st_mode & S_IXOTH)
        printf("x");
    else
        printf("-");
    printf("\t");
    printf("%lu\t", statbufls->st_nlink);
    printf("%s\t%s\t", temppwd->pw_name, tempgrp->gr_name);
    printf("%ld\t", statbufls->st_size);
    struct tm *time = localtime(&(statbufls->st_mtim.tv_sec));
    printf("%s\t%d\t%d:%d\t", months[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);
    if ((statbufls->st_mode & S_IFMT) == S_IFDIR)
        printf("%s%s%s\n", KGRN, filename, RESET);
    else if (statbufls->st_mode & S_IXUSR)
        printf("%s%s%s\n", KBLU, filename, RESET);
    else
        printf("%s\n", filename);
}

void printsingledir(int flag, char *dirname)
{
    char filepath[500];
    int namelen = strlen(dirname), total = 0;
    strcpy(filepath, dirname);

    if (statbufls == NULL)
    {
        printf(KRED "ls %s: Out of memory.\n" RESET, dirname);
        return;
    }

    struct dirent **namelist;
    int len;
    if (flag & 1)
        len = scandir(dirname, &namelist, NULL, alphasort);
    else
        len = scandir(dirname, &namelist, filter, alphasort);

    for (int i = 0; i < len; i++)
    {
        filepath[namelen] = '\0';
        strcat(filepath, "/");
        strcat(filepath, namelist[i]->d_name);
        stat(filepath, statbufls);
        total += statbufls->st_blocks / 2;
    }

    if (flag & 2)
    {
        printf("total %d\n", total);
        for (int i = 0; i < len; i++)
        {
            filepath[namelen] = '\0';
            strcat(filepath, "/");
            strcat(filepath, namelist[i]->d_name);

            printentry(filepath, namelist[i]->d_name);
            free(namelist[i]);
        }
    }
    else
    {
        for (int i = 0; i < len; i++)
        {
            filepath[namelen] = '\0';
            strcat(filepath, "/");
            strcat(filepath, namelist[i]->d_name);

            stat(filepath, statbufls);
            if ((statbufls->st_mode & S_IFMT) == S_IFDIR)
                printf("%s%s%s   ", KGRN, namelist[i]->d_name, RESET);
            else if (statbufls->st_mode & S_IXUSR)
                printf("%s%s%s   ", KBLU, namelist[i]->d_name, RESET);
            else
                printf("%s   ", namelist[i]->d_name);
            free(namelist[i]);
        }
        printf("\n");
    }
    free(namelist);
}

int listdirectory(int argc, char **argv, sysinfo *currsys)
{
    DIR *tempdir;
    int preventleak = 0;
    char tildefile[500];
    strcpy(tildefile, currsys->home_dir);
    int i = 1, j = 0;
    int flag = 0, flagcnt = 0, files = 0;
    statbufls = malloc(sizeof(struct stat));

    //  flag 10 for -l, 01 for -a and 11 for -la or -al
    while (argv[i] != NULL)
    {
        if (argv[i][0] == '-')
        {
            j = 1;
            if (argv[i][1] != '\0')
                flagcnt++;
            while (argv[i][j] != '\0')
            {
                if (argv[i][j] == 'a')
                    flag |= 1;
                else if (argv[i][j] == 'l')
                    flag |= 2;
                else
                {
                    printf(KRED "ls: Invalid option %s.\n" RESET, argv[i]);
                    return 1;
                }
                j++;
            }
        }
        i++;
    }

    if (argc - flagcnt <= 1)
    {
        argv[1] = malloc(2);
        argv[1][0] = '.';
        argv[1][1] = '\0';
        argv[2] = NULL;
        preventleak = 1;
    }

    i = 1;
    while (argv[i] != NULL)
    {
        if ((argv[i][0] == '-' && argv[i][1] == '\0') || (argv[i][0] != '-'))
        {
            if (argv[i][0] == '~')
            {
                strcat(tildefile, &(argv[i][1]));
                if ((tempdir = opendir(tildefile)) == NULL)
                {
                    if (stat(tildefile, statbufls) == -1)
                        printf(KRED "ls: cannot access '%s'" RESET ": No such file or directory\n", tildefile);
                }
                free(tempdir);

                strcpy(tildefile, currsys->home_dir);
            }

            else if ((tempdir = opendir(argv[i])) == NULL)
            {
                if (stat(argv[i], statbufls) == -1)
                    printf(KRED "ls: cannot access '%s'" RESET ": No such file or directory\n", argv[i]);
            }
            free(tempdir);
        }
        i++;
    }

    i = 1;
    while (argv[i] != NULL)
    {
        if ((argv[i][0] == '-' && argv[i][1] == '\0') || (argv[i][0] != '-'))
        {
            if (argv[i][0] == '~')
            {
                strcat(tildefile, &(argv[i][1]));
                if ((tempdir = opendir(tildefile)) == NULL)
                {
                    if (stat(tildefile, statbufls) != -1)
                    {
                        if (flag & 2)
                        {
                            printentry(tildefile, tildefile);
                        }
                        else
                        {
                            if ((statbufls->st_mode & S_IFMT) == S_IFDIR)
                                printf("%s%s%s  ", KGRN, tildefile, RESET);
                            else if (statbufls->st_mode & S_IXUSR)
                                printf("%s%s%s  ", KBLU, tildefile, RESET);
                            else
                                printf("%s  ", tildefile);
                        }
                        files = 1;
                    }
                }
                free(tempdir);
                strcpy(tildefile, currsys->home_dir);
            }

            else if ((tempdir = opendir(argv[i])) == NULL)
            {
                if (stat(argv[i], statbufls) != -1)
                {
                    if (flag & 2)
                    {
                        printentry(argv[i], argv[i]);
                    }
                    else
                    {
                        if ((statbufls->st_mode & S_IFMT) == S_IFDIR)
                            printf("%s%s%s   ", KGRN, argv[i], RESET);
                        else if (statbufls->st_mode & S_IXUSR)
                            printf("%s%s%s   ", KBLU, argv[i], RESET);
                        else
                            printf("%s   ", argv[i]);
                    }
                    files = 1;
                }
            }
            free(tempdir);
        }
        i++;
    }
    if (files)
        printf("\n");

    i = 1;
    while (argv[i] != NULL)
    {
        if ((argv[i][0] == '-' && argv[i][1] == '\0') || (argv[i][0] != '-'))
        {
            if (argv[i][0] == '~')
            {
                strcat(tildefile, &(argv[i][1]));
                if ((tempdir = opendir(tildefile)) != NULL)
                {
                    if (argc - flagcnt > 2)
                        printf("\n%s:\n", tildefile);
                    printsingledir(flag, tildefile);
                }
                free(tempdir);
                strcpy(tildefile, currsys->home_dir);
            }
            if ((tempdir = opendir(argv[i])) != NULL)
            {
                if (argc - flagcnt > 2)
                    printf("\n%s:\n", argv[i]);
                printsingledir(flag, argv[i]);
            }
            free(tempdir);
        }
        i++;
    }
    free(statbufls);
    if (preventleak)
        free(argv[1]);
    return 0;
}