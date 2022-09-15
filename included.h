#ifndef __INCLUDED_H
#define __INCLUDED_H

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>
#include <grp.h>
#include <sys/wait.h>
#include <ctype.h>

#define NUM_CMD 10
#define CMD_INC 5
#define DEF_BUF_SIZE 200
#define BUF_SZ_INC 100
#define DEF_HIST_SIZE 20
#define DEF_HIST_OUT 10
#define MAX_PIPES 100

#define KNRM  "\033[1;31m\x1B[0m"
#define KRED  "\033[1;31m\x1B[31m"
#define KGRN  "\033[1;31m\x1B[32m"
#define KYEL  "\033[1;31m\x1B[33m"
#define KBLU  "\033[1;31m\x1B[34m"
#define KMAG  "\033[1;31m\x1B[35m"
#define KCYN  "\033[1;31m\x1B[36m"
#define KWHT  "\033[1;31m\x1B[37m"
#define RESET "\033[1;31m\x1B[0m"

typedef struct _sysinfo {
    char *home_dir, *user, *OS, *curr_dir, *rel_path;
} sysinfo;

typedef struct _procinfo {
    char *procname;
    char *command;
    int pid, jobno, status; // 1 if running, 0 if stopped
} procinfo;

int outfd, infd, o_outfd, o_infd;
extern int outfd, infd, o_outfd, o_infd;

#endif