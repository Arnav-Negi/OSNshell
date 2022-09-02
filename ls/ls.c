#include "../included.h"
#include "ls.h"
struct stat *statbuf;
char *months[] = {"January  ", "February ", "March    ", "April    ", "May      ", "June     ", "July     ", "August   ", "September", "October  ", "November ", "December "};

int filter(const struct dirent *a)
{
    return a->d_name[0] != '.';
}

void printentry(char *filepath)
{
    
}

void printsingledir(int flag, char *dirname)
{
    struct passwd *temppwd;
    struct group *tempgrp;

    char filepath[500];
    int namelen = strlen(dirname), total = 0;
    strcpy(filepath, dirname);

    if (statbuf == NULL)
    {
        printf("ls %s: Out of memory.\n", dirname);
        return;
    }

    struct dirent **namelist;
    int len;
    if (flag & 1)
        len = scandir(dirname, &namelist, NULL, alphasort);
    else
        len = scandir(dirname, &namelist, filter, alphasort);

    for (int i = 0; i<len; i++)
    {
        filepath[namelen] = '\0';
            strcat(filepath, "/");
            strcat(filepath, namelist[i]->d_name);
        stat(filepath, statbuf);
        total += statbuf->st_blocks;
    }

    if (flag & 2)
    {
        printf("total %d\n", total);
        for (int i = 0; i < len; i++)
        {
            filepath[namelen] = '\0';
            strcat(filepath, "/");
            strcat(filepath, namelist[i]->d_name);

            stat(filepath, statbuf);
            tempgrp = getgrgid(statbuf->st_gid);
            temppwd = getpwuid(statbuf->st_uid);
            if ((statbuf->st_mode & S_IFMT) == S_IFDIR)
                printf("d");
            else
                printf("-");
            if (statbuf->st_mode & S_IRUSR)
                printf("r");
            else
                printf("-");
            if (statbuf->st_mode & S_IWUSR)
                printf("w");
            else
                printf("-");
            if (statbuf->st_mode & S_IXUSR)
                printf("x");
            else
                printf("-");
            if (statbuf->st_mode & S_IRGRP)
                printf("r");
            else
                printf("-");
            if (statbuf->st_mode & S_IWGRP)
                printf("w");
            else
                printf("-");
            if (statbuf->st_mode & S_IXGRP)
                printf("x");
            else
                printf("-");
            if (statbuf->st_mode & S_IROTH)
                printf("r");
            else
                printf("-");
            if (statbuf->st_mode & S_IWOTH)
                printf("w");
            else
                printf("-");
            if (statbuf->st_mode & S_IXOTH)
                printf("x");
            else
                printf("-");
            printf("\t");
            printf("%lu\t", statbuf->st_nlink);
            printf("%s\t%s\t", temppwd->pw_name, tempgrp->gr_name);
            printf("%ld\t", statbuf->st_size);
            struct tm *time = localtime(&(statbuf->st_mtim.tv_sec));
            printf("%s\t%d\t%d:%d\t", months[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);
            if ((statbuf->st_mode & S_IFMT) == S_IFDIR)
                printf("%s%s%s\n", KGRN, namelist[i]->d_name, RESET);
            else if (statbuf->st_mode & S_IXUSR)
                printf("%s%s%s\n", KBLU, namelist[i]->d_name, RESET);
            else
                printf("%s\n", namelist[i]->d_name);
        }
    }
    else
    {
        for (int i = 0; i < len; i++)
            printf("%s\n", namelist[i]->d_name);
    }
}

int listdirectory(int argc, char **argv)
{
    int i = 1, j = 0;
    int flag = 0, flagcnt = 0;
    statbuf = malloc(sizeof(struct stat));

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
                    printf("ls: Invalid option %s.\n", argv[i]);
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
    }

    i = 1;
    while (argv[i] != NULL)
    {
        if ((argv[i][0] == '-' && argv[i][1] == '\0') || (argv[i][0] != '-'))
        {
            if (opendir(argv[i]) == NULL)
            {
                stat(argv[i], statbuf);
                if (statbuf != NULL)
                    printf("'%s'\t", argv[i]);
            }
        }
        i++;
    }
    printf("\n");
    i = 1;
    while (argv[i] != NULL)
    {
        if ((argv[i][0] == '-' && argv[i][1] == '\0') || (argv[i][0] != '-'))
        {
            if (opendir(argv[i]) == NULL)
            {
                stat(argv[i], statbuf);
                if (statbuf == NULL)
                    printf("ls: cannot access '%s': No such file or directory\n", argv[i]);
            }
        }
        i++;
    }

    i = 1;
    while (argv[i] != NULL)
    {
        if ((argv[i][0] == '-' && argv[i][1] == '\0') || (argv[i][0] != '-'))
        {
            if (opendir(argv[i]) != NULL)
            {
                if (argc - flagcnt > 2)
                    printf("\n%s:\n", argv[i]);
                printsingledir(flag, argv[i]);
            }
        }
        i++;
    }
    free(statbuf);
    return 0;
}