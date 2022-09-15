#include "included.h"

int main()
{
    char **ar = (char **)malloc(24);
    ar[0] = "grep";
    ar[1] = "*";
    ar[2] = NULL;
    execvp(ar[0], ar);
}