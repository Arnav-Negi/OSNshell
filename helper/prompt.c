#include "prompt.h"

void prompt()
{
    free(currsys->rel_path);
    currsys->rel_path = convert_to_tilde(currsys->curr_dir, currsys);
    if (prevcmd_time != -1)
    {
        printf("%s\n<%s@%s:%s took %lds>%s", (err_prompt ? KRED : KCYN), currsys->user, currsys->OS, currsys->rel_path, time(NULL) - prevcmd_time, RESET);
        prevcmd_time = -1;
    }
    else
        printf("%s<%s@%s:%s>%s", (err_prompt || errno ? KRED : KCYN), currsys->user, currsys->OS, currsys->rel_path, RESET);
    err_prompt = 0, errno = 0;
    fflush(stdout);
}
