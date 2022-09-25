#include "included.h"

void func(int signum)
{
    printf("%d", signum);
}

int main()
{
    char name[200];
    while (1)
    {
        printf("Enter name ");
        scanf("%s", name);
        printf("Hello %s", name);
    }
}