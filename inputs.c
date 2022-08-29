#include "inputs.h"

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

            commands = (char **)malloc(sizeof(char *) * CMD_SZ);
            if (commands == NULL)
            {
                perror("Out of memory while taking input");
                exit(1);
            }
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

