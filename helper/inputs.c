#include "inputs.h"
#include "../signals/signal_handling.h"
#include "autosuggest.h"

extern int errno, outfd, infd, o_outfd, o_infd;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_termios) == -1)
    {
        // perror("tcsetattr");
        exit(1);
    }
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &og_termios) == -1)
    {
        // perror("tcgetattr");
        exit(1);
    }
    atexit(disableRawMode);
    struct termios raw = og_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    {
        // perror("tcsetattr");
        exit(1);
    }
}

char *convert_from_tilde(char *pathname, sysinfo *currsys)
{
    char *returnpath = malloc(strlen(pathname) + strlen(currsys->home_dir) + 10);
    strcpy(returnpath, currsys->home_dir);
    strcat(returnpath, "/");

    strcat(returnpath, pathname);

    return returnpath;
}

char *convert_to_tilde(char *pathname, sysinfo *currsys)
{
    int offset, offset_rel;
    char *newpath = malloc(sizeof(char) * (strlen(pathname) + 1));
    if (pathname == NULL)
    {
        printf(KRED "Out of memory\n" RESET);
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
                perror(KRED "Out of memory while taking input" RESET);
                exit(1);
            }
            for (int i = num_cmds - 1; i < CMD_SZ; i++)
                commands[i] = NULL;
        }
    }
    commands[num_cmds] = NULL;
    return commands;
}

char **take_input(sysinfo *currsys)
{
    // Take input and handle buffer OF. Parse multiple inputs.
    int BUF_SIZE = DEF_BUF_SIZE;
    int ptr = 0;
    char c;
    char **input = malloc(sizeof(char *));
    (*input) = (char *)malloc(sizeof(char) * BUF_SIZE);

    if ((*input) == NULL)
    {
        perror(KRED "out of memory while taking (*input).\n" RESET);
        free(input);
        exit(1);
    }
    setbuf(stdout, NULL);
    enableRawMode();
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (iscntrl(c))
        {
            switch (c)
            {
            case 10: // newline
                (*input)[ptr++] = '\0';
                printf("\n");
                disableRawMode();
                return input;
                break;
            case 27: // arrow key
                read(STDIN_FILENO, &c, 1);
                read(STDIN_FILENO, &c, 1);
                break;
            case 4: // EOT (Ctrl D)
                free((*input));
                free(input);
                return NULL;
                break;
            case 127: // backspace
                if (ptr > 0)
                {
                    (*input)[--ptr] = '\0';
                    printf("\b \b");
                }
                break;
            case 9: // tab, autosuggest
                (*input)[ptr] = '\0';
                autocomplete(input);
                ptr = strlen(*input);
                break;
            default:
                break;
            }
        }
        else
        {
            (*input)[ptr++] = c;
            printf("%c", c);
        }

        // dymnamic memory alloc
        if (ptr >= BUF_SIZE - 3)
        {
            BUF_SIZE += BUF_SZ_INC;
            (*input) = realloc((*input), BUF_SIZE);

            if ((*input) == NULL)
            {
                perror(KRED "out of memory while taking (*input).\n" RESET);
                free(input);
                exit(1);
            }
        }
    }
    disableRawMode();
    return input;
}

int changeIO(int inputfile, int outputfile)
{
    if (inputfile != -1)
    {
        if (dup2(STDIN_FILENO, o_infd) < 0)
        {
            printf(KRED "Couldn't redirect I/O.\n" RESET);
            return 1;
        }
        if (dup2(inputfile, STDIN_FILENO) < 0)
        {
            printf(KRED "Couldn't redirect I/O.\n" RESET);
            return 1;
        }
        infd = inputfile;
    }
    if (outputfile != -1)
    {
        if (dup2(STDOUT_FILENO, o_outfd) < 0)
        {
            printf(KRED "Couldn't redirect I/O.\n" RESET);
            return 1;
        }
        if (dup2(outputfile, STDOUT_FILENO) < 0)
        {
            printf(KRED "Couldn't redirect I/O.\n" RESET);
            return 1;
        }
        outfd = outputfile;
    }

    return 0;
}

int parseIO(int argc, char **args)
{
    int imode = 0, omode = 0, newargc = 0;
    int infile = -1, outfile = -1;
    // Handle I/O redirection.
    for (int i = 0; i < argc; i++)
    {
        if (omode == 1)
        {
            if ((outfile = open(args[i], O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0)
            {
                perror(KRED "Redirect error" RESET);
                return -1;
            }
            if (changeIO(-1, outfile) == 1)
                return -1;
            omode = 0;
        }
        else if (omode == 2)
        {
            if ((outfile = open(args[i], O_CREAT | O_APPEND, 0644)) < 0)
            {
                perror(KRED "Redirect error" RESET);
                return -1;
            }
            if (changeIO(-1, outfile) == 1)
                return -1;
            omode = 0;
        }
        else if (imode)
        {
            if ((infile = open(args[i], O_RDONLY)) < 0)
            {
                perror(KRED "Redirect error" RESET);
                return -1;
            }
            if (changeIO(infile, -1) == 1)
                return -1;
            imode = 0;
        }
        else if (strcmp(args[i], ">") == 0)
            omode = 1;
        else if (strcmp(args[i], ">>") == 0)
            omode = 2;
        else if (strcmp(args[i], "<") == 0)
            imode = 1;
        else
        {
            args[newargc++] = args[i];
        }
    }

    args[newargc] = NULL;
    return newargc;
}
