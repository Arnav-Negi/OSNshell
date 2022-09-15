#include "jobs.h"

int jobcmp(const void *j1, const void *j2) {
    int i = 0;
    char *s1 = (*(procinfo **)j1)->procname;
    char *s2 = (*(procinfo **)j2)->procname;
    while (i < strlen(s1) && i<strlen(s2) && tolower(s1[i]) == tolower(s2[i])) i++;
    if (i == strlen(s1) && i != strlen(s2)) return 0;
    if (i == strlen(s2) && i != strlen(s1)) return 1;
    if (i == strlen(s1) && i == strlen(s2)) return 0;
    if (tolower(s1[i]) > tolower(s2[i])) return 1;
    else return 0;
}

int jobs(procinfo *array[], int argc, char **args)
{
    int i = 1, flag = 0;

    while (args[i] != NULL)
    {
        if (strcmp(args[i], "-r") == 0)
            flag |= 1;
        else if (strcmp(args[i], "-s") == 0)
            flag |= 2;
        else
        {
            printf(KRED "Error: wrong arguments. Only -r or -s allowed.\n" RESET);
            return 1;
        }
        i++;
    }
    if (!flag)
        flag = 3;
    procinfo *display[1000];
    int displaycnt = 0;
    display[0] = NULL;
    i = 0;
    while (i < 1000)
    {
        if (array[i] != NULL)
        {
            if ((array[i]->status == 1 && (flag & 1)) || (array[i]->status == 0 && (flag&2)))
                display[displaycnt++] = array[i];
        }
        i++;
    }

    qsort(display, displaycnt, sizeof(procinfo *), jobcmp);
    for (int i = 0; i<displaycnt; i++) {
        printf("[%d] %s %s [%d]\n", display[i]->jobno, (display[i]->status?"Running":"Stopped"), display[i]->command, display[i]->pid);
    }
    
    return 0;
}