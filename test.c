#include "included.h"

char hm[] = "1902";

int main()
{
    char **endptr = malloc(sizeof(char *));
    endptr[0] = NULL;
    int x = strtoll(hm, endptr, 10);
    if (endptr[0] != NULL)
    {
        printf("Error pinfo: argument not a valid pid\n");
    }
}