#define __MAIN__
#include "headers.h"

struct termios og_termios;
extern int errno;

int outfd, infd, o_outfd, o_infd;
int err_prompt, pipeflag, isrunning, pipesleft;
procinfo fgpid;
long int prevcmd_time = -1;
char *prevcd;
sysinfo *currsys;

procinfo *bgpid[1000] = {NULL}, *piped_procs[MAX_PIPES] = {NULL};


// PROTOTYPES
void rembg(int signum);
int createproc(int isbg, char *cmd, int argc, char **args, int ispipes);
int runcommand(int isbg, char *cmd, int argc, char **args);
int runpipes(int isbg, char *cmd, sysinfo *currsys);
int handle_inputs(char *line);
int shell_loop();

int createproc(int isbg, char *cmd, int argc, char **args, int ispipes)
{
    argc = parseIO(argc, args);
    if (argc == -1)
    {
        return 1;
    }

    if (args[0] == NULL)
    {
        printf("\n");
        prompt();
        return 0;
    }
    if (strcmp(args[0], "cd") == 0)
    {
        err_prompt = cd(args, currsys);
        return 0;
    }
    if (strcmp(args[0], "quit") == 0 || strcmp(args[0], "exit") == 0)
    {
        return 2;
    }
    else if (strcmp(args[0], "jobs") == 0)
    {
        err_prompt = jobs(argc, args);
        return 0;
    }
    else if (strcmp(args[0], "sig") == 0)
    {
        err_prompt = sig(argc, args);
        return 0;
    }
    else if (strcmp(args[0], "fg") == 0)
    {
        err_prompt = fg(argc, args);
        return 0;
    }
    else if (strcmp(args[0], "bg") == 0)
    {
        err_prompt = bg(argc, args);
        return 0;
    }
    if (!isbg)
    {
        prevcmd_time = time(NULL);
        if (prevcmd_time == -1)
        {
            perror(KRED "Time error" RESET);
            exit(1);
        }
    }

    int parentpid = getpid();
    int pid = fork();
    int status;

    if (pid < 0)
    {
        perror(KRED "fork error" RESET);
        return (1);
    }

    if (!pid)
    {
        if (!ispipes)
            setpgid(0, 0);
        status = runcommand(isbg, cmd, argc, args);
        exit(status);
    }
    else
    {
        if (!ispipes)
            setpgid(pid, pid);

        if (isbg == 1)
        {
            newbg(pid, args[0], cmd, 1);
        }
        else
        {
            fgpid.pid = pid, fgpid.command = malloc(strlen(cmd) + 1), fgpid.status = 1, fgpid.procname = malloc(strlen(args[0]) + 1);
            strcpy(fgpid.procname, args[0]), strcpy(fgpid.command, cmd);

            signal(SIGTTIN, SIG_IGN);
            signal(SIGTTOU, SIG_IGN);

            tcsetpgrp(STDIN_FILENO, pid);
            tcsetpgrp(STDOUT_FILENO, pid);

            fgpid.status = 1;
            isrunning = 1;
            while (isrunning)
            {
            }
            if (infd != -1)
            {
                close(infd);
                infd = -1;
                dup2(o_infd, STDIN_FILENO);
            }
            if (outfd != -1)
            {
                close(outfd);
                outfd = -1;
                dup2(o_outfd, STDOUT_FILENO);
            }
            fgpid.pid = -1;
            free(fgpid.procname);
            free(fgpid.command);

            tcsetpgrp(STDIN_FILENO, parentpid);
            tcsetpgrp(STDOUT_FILENO, parentpid);

            signal(SIGTTIN, SIG_DFL);
            signal(SIGTTOU, SIG_DFL);

            if (WIFSTOPPED(status))
                return 0;

            if (WEXITSTATUS(status) == 1)
            {
                err_prompt = 1;
                return 1;
            }
        }
    }

    return 0;
}

int runcommand(int isbg, char *cmd, int argc, char **args)
{
    char *temp;
    int status = 0;

    //     args[0] gives command, rest are arguments.Handle commands here using strcmp.
    if (strcmp(args[0], "pwd") == 0)
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
        err_prompt = listdirectory(argc, args, currsys);
    }
    else if (strcmp(args[0], "pinfo") == 0)
    {
        if (argc > 2)
            printf(KRED "Error pinfo: too many arguments\n" RESET);
        else if (args[1] == NULL)
            pinfo(-1, currsys);
        else
        {
            char **endptr = malloc(sizeof(char *));
            int x = strtoll(args[1], endptr, 10);
            if (endptr[0][0] != '\0')
            {
                printf(KRED "Error pinfo: argument not a valid pid\n" RESET);
            }
            else
                err_prompt = pinfo(x, currsys);
            free(endptr);
        }
    }
    else if (strcmp(args[0], "discover") == 0)
    {
        err_prompt = discover(argc, args, currsys);
    }
    else if (strcmp(args[0], "history") == 0)
    {
        print_history(currsys);
    }
    else
    {
        if (execvp(args[0], args) < 0)
        {
            perror(KRED "Command not found" RESET);
            exit(1);
        }
        exit(0);
    }
    fflush(stdout);
    return status;
}

int runpipes(int isbg, char *cmd, sysinfo *currsys)
{
    int filepipe[MAX_PIPES][2], childpid, argc = 0, status;
    char **pipecmds, **args;
    int numpipes = 0;
    int done = 0;

    pipecmds = tokenize(cmd, "|");

    while (pipecmds[numpipes] != NULL)
        numpipes++;

    if (numpipes == 1)
    {
        args = tokenize(pipecmds[done], " \t");
        while (args[argc] != NULL)
            argc++;

        status = createproc(isbg, cmd, argc, args, isbg);
        free(args), free(pipecmds);
        return status;
    }

    if (numpipes > MAX_PIPES)
    {
        printf(KRED "Too many piped commands.\n" RESET);
        free(pipecmds);
        return 1;
    }
    pipesleft = numpipes;
    if (!isbg)
        prevcmd_time = time(NULL);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    while (done < numpipes)
    {
        if (done != numpipes - 1 && pipe(filepipe[done]) == -1)
        {
            printf(KRED "Couldn't create pipe.\n" RESET);
            free(pipecmds);
            return 1;
        }

        if ((childpid = fork()) == -1)
        {
            printf(KRED "Fork error.\n" RESET);
            free(pipecmds);
            return 1;
        }
        args = tokenize(pipecmds[done], " \t");
        if (childpid == 0)
        {
            // child process.
            // Takes input from pipe[done + 1][0] and output to pipe[done][1].
            // Special case : done == 0 or numpipes.

            if (done != 0)
            {
                setpgid(getpid(), pipeflag);
                if (done != numpipes - 1)
                {
                    changeIO(filepipe[done - 1][0], filepipe[done][1]);
                    close(filepipe[done][0]);
                }
                else
                {
                    changeIO(filepipe[done - 1][0], -1);
                }
            }
            else
            {
                setpgrp();
                tcsetpgrp(STDIN_FILENO, getpid());
                tcsetpgrp(STDOUT_FILENO, getpid());
                changeIO(-1, filepipe[done][1]);
                close(filepipe[done][0]);
            }

            // Running command.
            while (args[argc] != NULL)
                argc++;

            status = createproc(0, cmd, argc, args, 1);
            free(args);
            // Closing pipe.
            if (done != numpipes - 1)
                close(filepipe[done][1]);
            if (done > 0)
                close(filepipe[done - 1][0]);
            exit(status);
        }
        else
        {
            if (isbg)
                newbg(childpid, args[0], cmd, 1);
            if (done == 0)
                pipeflag = childpid;
            int pipeptr = 0;
            while (pipeptr < MAX_PIPES)
            {
                if (piped_procs[pipeptr] == NULL)
                {
                    piped_procs[pipeptr] = malloc(sizeof(procinfo));
                    piped_procs[pipeptr]->pgpid = pipeflag;
                    piped_procs[pipeptr]->pid = childpid;
                    break;
                }
                pipeptr++;
            }

            // parent process.
            // close unused pipe fd.
            
            if (done != numpipes - 1)
                close(filepipe[done][1]);
            if (done > 0)
                close(filepipe[done - 1][0]);
        }
        free(args);
        done++;
    }
    while (pipesleft)
    {
    }
    tcsetpgrp(STDIN_FILENO, getpid());
    tcsetpgrp(STDOUT_FILENO, getpid());
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    fflush(stdin), fflush(stdout);
    free(pipecmds);
    return 0;
}

int handle_inputs(char *line)
{
    addhistory(line, currsys);
    char **commands, **requests;

    commands = tokenize(line, ";");
    int i = 0, j = 0, bg = 0, totalbg, status = 0;
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
            if (bg > 0 ? totalbg - bg + 1 : 0)
                isrunning = 1;
            else
                isrunning = 0;
            status = runpipes((bg > 0 ? totalbg - bg + 1 : 0), requests[j], currsys);
            bg--;
            j++;
        }
        free(requests);
    }
    free(commands);
    return status;
}

int shell_loop()
{
    char *line, **input;

    int status = 0;
    do
    {
        prompt();
        input = take_input(currsys);
        if (input == NULL)
        {
            status = 2;
            break;
        }
        line = *input;
        free(input);
        isrunning = 0;
        status = handle_inputs(line);
        free(line);
    } while (status != 2);
    return status;
}

int main(int argc, char **argv)
{
    signal(SIGCHLD, rembg);
    signal(SIGTSTP, DoNothing);
    signal(SIGINT, DoNothing);

    err_prompt = 0;
    fgpid.pid = -1;
    int retval = 0;
    infd = -1, outfd = -1, o_outfd = 500, o_infd = 501;
    isrunning = 0;

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

    retval = shell_loop();
    free(info);
    free(currsys->rel_path), free(currsys->curr_dir), free(currsys->home_dir);
    free(utsbuf);
    free(prevcd);
    free(currsys);
    if (fgpid.pid != -1)
    {
        free(fgpid.procname), free(fgpid.command);
    }
    if (retval == 2)
        return 0;
    return 1;
}