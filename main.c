#include "included.h"
#include "inputs.h"
#include "pwd.h"
#include "echo.h"

extern int errno;
sysinfo *currsys;

void prompt()
{
    int offset, offset_rel;

    if (strncmp(currsys->curr_dir, currsys->home_dir, strlen(currsys->home_dir)) == 0)
    {
        offset = strlen(currsys->home_dir);
        offset_rel = 1;
        currsys->rel_path[0] = '~';
    }
    else
    {
        offset = 0;
        offset_rel = 0;
    }

    currsys->rel_path[0] = '~';
    for (int i = 0;; i++)
    {
        currsys->rel_path[i + offset_rel] = currsys->curr_dir[i + offset];
        if (currsys->rel_path[i + offset_rel] == '\0')
            break;
    }
    printf("<%s@%s:%s>", currsys->user, currsys->OS, currsys->rel_path);
    fflush(stdout);
}

int handle_inputs(char *line)
{
    char **commands, **requests, **args;

    commands = tokenize(line, ";");
    int i = 0, j = 0;
    while (commands[i] != NULL)
    {
        // SPEC 4 : Handle '&'.
        requests = tokenize(commands[i++], "&");
        j = 0;
        while (requests[j] != NULL)
        {
            args = tokenize(requests[j++], " \t");

            // args[0] gives command, rest are arguments.
            // Handle commands here using strcmp.
            if (strcmp(args[0], "cd") == 0)
            {
                // cd(args);
            }
            if (strcmp(args[0], "pwd") == 0)
            {
                pwd();
            }
            if (strcmp(args[0], "echo") == 0)
            {
                echo(args);
            }
            printf("\n");
            free(args);
        }
        free(requests);
    }
    free(commands);
    return 0;
}

void shell_loop()
{
    char *line;
    int status;

    do
    {
        prompt();
        line = take_input(currsys);
        status = handle_inputs(line);
        free(line);
    } while (status == 0);
}

int main(int argc, char **argv)
{
    currsys = (sysinfo *)malloc(sizeof(sysinfo));
    currsys->rel_path = (char *)malloc(sizeof(char) * 1024);

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

    currsys->OS = malloc(1024);
    gethostname(currsys->OS, 1024);

    currsys->user = getlogin();
    shell_loop();

    return 0;
}