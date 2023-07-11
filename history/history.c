#include "history.h"


void print_history(sysinfo *currsys)
{
    char **commandlist = malloc(sizeof(char *) * (DEF_HIST_SIZE + 3));
    int l = getcommands(commandlist, currsys);

    for (int i = 0; i < l; i++)
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
    while (getline(&commandlist[i], &n, readfile) >= 0)
    {
        i++;
        n = 0;
        commandlist[i] = NULL;
    }
    free(commandlist[i]);
    commandlist[i] = NULL;
    free(path);
    fclose(readfile);
    return i;
}

int comparelines(char *line1, char *line2)
{
    char **word1 = malloc(sizeof(char *) * 100);
    char **word2 = malloc(sizeof(char *) * 100);
    int i = 0;
    word1[i] = strtok(line1, " \t\n");
    while (word1[i] != NULL)
    {
        i++;
        word1[i] = strtok(NULL, " \t\n");
    }

    int j = 0;
    word2[j] = strtok(line2, " \t\n");
    while (word2[j] != NULL)
    {
        j++;
        word2[j] = strtok(NULL, " \t\n");
    }

    if (i != j)
    {
        free(word1), free(word2);
        return 0;
    }
    for (int k = 0; k < i; k++)
    {
        if (strcmp(word1[k], word2[k]) != 0)
        {
            free(word1), free(word2);
            return 0;
        }
    }
    free(word1), free(word2);
    return 1;
}

int addhistory(char *newcmd, sysinfo *currsys)
{
    char **commandlist = malloc(sizeof(char *) * (DEF_HIST_SIZE + 3));
    int lines = getcommands(commandlist, currsys);

    char *path = malloc(strlen(currsys->home_dir) + 25);
    strcpy(path, currsys->home_dir);
    strcat(path, "/history/.history.txt");

    if (lines > 0)
    {
        char *line1 = malloc(strlen(newcmd) + 5);
        strcpy(line1, newcmd);
        strcat(line1, "\n");
        char *line2 = malloc(strlen(commandlist[lines - 1]) + 1);
        strcpy(line2, commandlist[lines - 1]);

        if (comparelines(line1, line2))
        {
            for (int i = 0; i < lines; i++)
            {
                free(commandlist[i]);
            }
            free(commandlist), free(path), free(line1), free(line2);

            return 1;
        }

        free(line1), free(line2);
    }

    if (lines >= DEF_HIST_SIZE)
    {
        // replace top line.
        FILE *writefile = fopen(path, "w");
        int i = lines - DEF_HIST_SIZE + 1;
        for (int temp = 0; temp<i; temp++) free(commandlist[temp]);
        while (i < lines)
        {
            fprintf(writefile, "%s", commandlist[i]);
            free(commandlist[i]);
            i++;
        }
        fprintf(writefile, "%s\n", newcmd);
        fflush(writefile);
        fclose(writefile);
    }
    else
    {
        // Add on to file on top.
        FILE *appendfile = fopen(path, "a");
        fprintf(appendfile, "%s\n", newcmd);
        for (int i = 0; i < lines; i++)
        {
            free(commandlist[i]);
        }
        fflush(appendfile);
        fclose(appendfile);
    }

    free(commandlist), free(path);
    return 1;
}