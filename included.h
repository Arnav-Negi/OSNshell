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

#define NUM_CMD 10
#define CMD_INC 5
#define DEF_BUF_SIZE 200;
#define BUF_SZ_INC 100;

typedef struct _sysinfo {
    char *home_dir, *user, *OS, *curr_dir, *rel_path;
} sysinfo;

#endif