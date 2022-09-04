#include "included.h"
#include "inputs.h"
#include "builtin/workingdir.h"
#include "builtin/echo.h"
#include "builtin/cd.h"
#include "ls/ls.h"
#include "pinfo/pinfo.h"

extern int errno;
long int prevcmd_time = -1;
char *prevcd;
sysinfo *currsys;

procinfo *bgpid[1000] = {NULL};

// PROTOTYPES
int newbg(int pid, char *proccess_name);
void rembg(int signum);
int createproc(int isbg, int argc, char **args);
void prompt();
int runcommand(int isbg, int argc, char **args, sysinfo *currsys);
int handle_inputs(char *line);
void shell_loop();

int newbg(int pid, char *proccess_name)
{
    if (pid == 0)
        return 0;

    int i = 0;
    while (i < 1000 && bgpid[i] != NULL)
        i++;
    if (i == 1000)
    {
        printf("Process limit reached.\n");
        exit(1);
    }
    bgpid[i] = (procinfo *)malloc(sizeof(procinfo));
    bgpid[i]->pid = pid;
    bgpid[i]->procname = (char *)malloc(strlen(proccess_name) + 1);
    strcpy(bgpid[i]->procname, proccess_name);

    return 0;
}

void rembg(int signum)
{
    int status;
    int pid = waitpid(-1, &status, WNOHANG);

    int i = 0;
    if (pid == 0)
        return;
    while (i < 1000)
    {
        if (bgpid[i] == NULL)
            i++;
        else if (bgpid[i]->pid == pid)
            break;
    }
    if (i == 1000)
        return;

    if (pid > 0)
    {
        if (WIFSIGNALED(status))
        {
            printf("\n%s with pid %d exited abnormally\n", bgpid[i]->procname, pid);
        }
        if (WIFEXITED(status))
        {
            printf("\n%s with pid %d exited normally\n", bgpid[i]->procname, pid);
        }
        prompt();
    }

    if (bgpid[i]->procname != NULL)
        free(bgpid[i]->procname);
    free(bgpid[i]);
    bgpid[i] = NULL;
}

int createproc(int isbg, int argc, char **args)
{
    int pid = fork();
    int status;

    if (pid < 0)
    {
        perror("fork error");
        return (1);
    }

    if (!pid)
    {
        if (execvp(args[0], args) < 0)
        {
            perror("File not found");
            exit(1);
        }
    }
    else
    {
        if (isbg)
        {
            printf("[%d] %d\n", isbg, pid);
            newbg(pid, args[0]);
        }
        else
        {
            waitpid(pid, &status, 0);
        }
    }

    return 0;
}

void prompt()
{
    free(currsys->rel_path);
    currsys->rel_path = convert_to_tilde(currsys->curr_dir, currsys);
    if (prevcmd_time != -1)
    {
        printf("<%s@%s:%s took %lds>", currsys->user, currsys->OS, currsys->rel_path, time(NULL) - prevcmd_time);
        prevcmd_time = -1;
    }
    else 
        printf("<%s@%s:%s>", currsys->user, currsys->OS, currsys->rel_path);
    fflush(stdout);
}

int runcommand(int isbg, int argc, char **args, sysinfo *currsys)
{
    char *temp;
    if (args[0] == NULL)
    {
        return 1;
    }

    if (!isbg)
    {
        prevcmd_time = time(NULL);
        if (prevcmd_time == -1)
        {
            perror("Time error");
            exit(1);
        }
    }

    //     args[0] gives command, rest are arguments.Handle commands here using strcmp.
    if (strcmp(args[0], "cd") == 0)
    {
        cd(args, currsys);
    }
    else if (strcmp(args[0], "pwd") == 0)
    {
        temp = pwd();
        printf("%s\n", temp);
        free(temp);
    }
    else if (strcmp(args[0], "echo") == 0)
    {
        echo(args);
    }
    else if (strcmp(args[0], "ls") == 0)
    {
        listdirectory(argc, args, currsys);
    }
    else if (strcmp(args[0], "pinfo") == 0)
    {
        if (argc > 2)
            printf("Error pinfo: too many arguments\n");
        else if (args[1] == NULL)
            pinfo(-1, bgpid, currsys);
        else
        {
            char **endptr = malloc(sizeof(char *));
            int x = strtoll(args[1], endptr, 10);
            if (endptr[0][0] != '\0')
            {
                printf("Error pinfo: argument not a valid pid\n");
            }
            else
                pinfo(x, bgpid, currsys);
            free(endptr);
        }
    }
    else
    {
        createproc(isbg, argc, args);
    }
    return 0;
}

int handle_inputs(char *line)
{
    char **commands, **requests, **args;

    commands = tokenize(line, ";");
    int i = 0, j = 0, argc, bg = 0, totalbg;
    while (commands[i] != NULL)
    {
        // SPEC 4 : Handle '&'.
        j = 0, bg = 0;
        char lastchar = '\0';
        while (commands[i][j] != '\0')
        {
            if (commands[i][j] == '&')
            {
                bg++;
                if (lastchar == '&')
                    return 1;
            }
            lastchar = commands[i][j++];
        }
        totalbg = bg;

        requests = tokenize(commands[i++], "&");
        j = 0;
        while (requests[j] != NULL)
        {
            args = tokenize(requests[j++], " \t");
            argc = 0;
            while (args[argc] != NULL)
            {
                argc++;
            }

            runcommand((bg > 0 ? totalbg - bg + 1 : 0), argc, args, currsys);
            bg--;
            free(args);
            free(requests[j]);
        }
        free(requests);
        free(commands[i]);
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
    signal(SIGCHLD, rembg);

    currsys = (sysinfo *)malloc(sizeof(sysinfo));
    currsys->rel_path = (char *)malloc(sizeof(char) * 1024);

    // username
    struct stat *info = malloc(sizeof(struct stat));
    stat(".", info);
    struct passwd *uid;
    uid = getpwuid(info->st_uid);
    currsys->user = uid->pw_name;

    // system name
    struct utsname *utsbuf = (struct utsname *)malloc(sizeof(struct utsname));
    uname(utsbuf);
    currsys->OS = utsbuf->nodename;

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

    // initialise prevcd
    prevcd = (char *)malloc(BUF);
    getcwd(prevcd, BUF);

    shell_loop();
    free(info);
    free(currsys->rel_path), free(currsys->curr_dir), free(currsys->home_dir);
    free(utsbuf);
    free(currsys);
    return 0;
}