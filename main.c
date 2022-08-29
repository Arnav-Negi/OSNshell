#include "included.h"

extern int errno;
sysinfo *currsys;
struct utsname* utsbuf;

#define DEF_BUF_SIZE 200;
#define BUF_SZ_INC 100;

void prompt()
{
    char *rel_path = (char *)malloc(sizeof(char) * 1024);
    int offset, offset_rel;

    if (strncmp(currsys->curr_dir, currsys->home_dir, strlen(currsys->home_dir)) == 0)
    {
        offset = strlen(currsys->home_dir);
        offset_rel = 1;
        rel_path[0] = '~';
    }
    else
    {
        offset = 0;
        offset_rel = 0;
    }

    rel_path[0] = '~';
    for (int i = 0;; i++)
    {
        rel_path[i + offset_rel] = currsys->curr_dir[i + offset];
        if (rel_path[i + offset_rel] == '\0')
            break;
    }
    printf("<%s@%s:%s>", currsys->user, currsys->OS, rel_path);
    fflush(stdout);
}

char **parse_input(char *command)
{
    char **args = (char **)malloc(sizeof(char *) * 10);
    strtok(command, " ");
}

char *take_input()
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
        prompt();
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
}

void shell_loop()
{
    char *line, *command;
    char **args;
    int status;

    do
    {
        line = take_input();

        command = strtok(line, ";&");

        free(line), free(command), free(args);
    } while (status);
}

int main(int argc, char **argv)
{
    currsys = (sysinfo *)malloc(sizeof(sysinfo));

    // set home directory.
    int BUF = DEF_BUF_SIZE;
    
    currsys->curr_dir = (char *)malloc(BUF);
    while (getcwd(currsys->curr_dir, BUF) == NULL)
    {
        BUF += BUF_SZ_INC;
        currsys->curr_dir = (char *)realloc(currsys->curr_dir, BUF);
    }
    currsys->home_dir = (char *)malloc(BUF);
    getcwd(currsys->home_dir, BUF);

    currsys->user = malloc(1024);
    gethostname(currsys->user, 1024);

    utsbuf = (struct utsname*) malloc(sizeof(struct utsname));
    uname(utsbuf);

    currsys->OS = utsbuf->sysname;
    shell_loop();

    return 0;
}