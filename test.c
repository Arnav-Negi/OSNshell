#include "included.h"

int main()
{
    struct stat *statbuf = malloc(sizeof(struct stat));
    char *pwd = malloc(100);
    getcwd(pwd, 100);
    int i = stat("ls", statbuf);
    if (i == -1) perror("hmm");
}