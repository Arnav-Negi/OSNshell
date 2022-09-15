#include "included.h"

int main()
{
    int pid = fork();
    if (pid) {
        printf("Hey\n");
    }
    else {
        printf("yoo\n");
    }
}