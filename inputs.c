#include "inputs.h"

char *convert_from_tilde(char *pathname, sysinfo *currsys)
{
    char *returnpath = malloc(strlen(pathname) + strlen(currsys->home_dir) + 10);
    strcpy(returnpath, currsys->home_dir);
    strcat(returnpath, "/");
    if (strlen(pathname) > 2)
        strcat(returnpath, pathname);
    
    return returnpath;
}

char *convert_to_tilde(char *pathname, sysinfo *currsys)
{
    int offset, offset_rel;
    char *newpath = malloc(sizeof(char) * (strlen(pathname) + 1));
    if (pathname == NULL)
    {
        printf("Out of memory\n");
        exit(1);
    }
    if (strncmp(pathname, currsys->home_dir, strlen(currsys->home_dir)) == 0)
    {
        offset = strlen(currsys->home_dir);
        offset_rel = 1;
        newpath[0] = '~';
    }
    else
    {
        offset = 0;
        offset_rel = 0;
    }

    newpath[0] = '~';
    for (int i = 0;; i++)
    {
        newpath[i + offset_rel] = pathname[i + offset];
        if (newpath[i + offset_rel] == '\0')
            break;
    }

    return newpath;
}

char **tokenize(char *line, char *delim)
{
    char **commands = (char **)malloc(sizeof(char *) * NUM_CMD);
    char *cmd;
    int num_cmds = 0, CMD_SZ = NUM_CMD;
    cmd = strtok(line, delim);
    while (cmd != NULL)
    {
        commands[num_cmds++] = cmd;
        cmd = strtok(NULL, delim);

        if (num_cmds >= CMD_SZ)
        {
            CMD_SZ += CMD_INC;

            commands = realloc(commands, sizeof(char *) * CMD_SZ);
            if (commands == NULL)
            {
                perror("Out of memory while taking input");
                exit(1);
            }
            for (int i = num_cmds - 1; i < CMD_SZ; i++)
                commands[i] = NULL;
        }
    }
    commands[num_cmds] = NULL;
    return commands;
}

char *take_input(sysinfo *currsys)
{
    // Take input and handle buffer OF. Parse multiple inputs.
    int BUF_SIZE = DEF_BUF_SIZE;
    int ptr = 0;
    char c;
    char *input = (char *)malloc(sizeof(char) * BUF_SIZE);

    if (input == NULL)
    {
        perror("out of memory while taking input.\n");
        exit(1);
    }

    while (1)
    {
        scanf("%c", &c);

        if (c == EOF || c == '\n')
        {
            input[ptr] = '\0';
            return input;
        }
        else
        {
            input[ptr++] = c;
        }

        // dymnamic memory alloc
        if (ptr >= BUF_SIZE)
        {
            BUF_SIZE += BUF_SZ_INC;
            input = realloc(input, BUF_SIZE);

            if (input == NULL)
            {
                perror("out of memory while taking input.\n");
                exit(1);
            }
        }
    }

    return input;
}
