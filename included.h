#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/utsname.h>

typedef struct sysinfo {
    char *home_dir, *user, *OS, *curr_dir;
} sysinfo;