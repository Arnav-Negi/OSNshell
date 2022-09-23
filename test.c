#include "included.h"

void func(int signum)
{
    printf("%d", signum);
}

int main()
{
    printf("start\n");
    signal(SIGCHLD, func);
    kill(getpid(), SIGCHLD);
    printf("after kill.\n");
}