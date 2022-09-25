#include "autosuggest.h"
#include "prompt.h"

char *path, *dirpath;
int start, end;

int tabfilter(const struct dirent *a)
{
    if (strncmp(a->d_name, path, strlen(path)) == 0)
        return 1;
    else
        return 0;
}

int autocomplete(char **line)
{
    struct dirent **namelist;

    start = strlen(*line) - 1;
    end = strlen(*line) - 1;
    while (start >= 0 && (*line)[start] != ' ')
        start--;
    start++;

    while (end >= 0 && (*line)[end] != '/' && (*line)[end] != ' ')
        end--;
    end++;

    path = malloc(strlen(*line) + 256);
    strcpy(path, &(*line)[end]);
    dirpath = malloc(strlen(*line) + 5);
    strcpy(dirpath, path);
    dirpath[end - start] = '\0';

    if (dirpath[0] == '\0')
    {
        dirpath[0] = '.';
        dirpath[1] = '\0';
    }

    int numdirs;
    numdirs = scandir(dirpath, &namelist, tabfilter, alphasort);
    char buffer[256] = {0};
    int ptr = end - start, valid = 0;
    while (1)
    {
        valid = 1;
        for (int i = 0; i < numdirs; i++)
        {
            if (ptr >= 255)
            {
                valid = 0;
                break;
            }
            if (namelist[i]->d_name[ptr] == '\0' || namelist[i]->d_name[ptr] != namelist[0]->d_name[ptr])
            {
                valid = 0;
                break;
            }
        }
        if (numdirs > 0 && valid)
        {
            buffer[ptr - (end - start)] = namelist[0]->d_name[ptr];
            ptr++;
            buffer[ptr - (end - start)] = '\0';
        }
        else
            break;
    }
    if (numdirs > 1)
    {
        printf("\n");
        for (int i = 0; i < numdirs; i++)
        {
            printf("%s\t", namelist[i]->d_name);
            free(namelist[i]);
        }
    }
    else if (numdirs == 1) free(namelist[0]);
    free(namelist);
    int i = 0;
    while (buffer[i] != '\0')
    {
        (*line)[end++] = buffer[i++];
        (*line)[end] = '\0';
    }
    printf("\n");
    prompt();
    printf("%s", *line);
    free(dirpath);
    free(path);
    return 1;
}