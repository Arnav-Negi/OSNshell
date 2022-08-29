#include "included.h"
#include <pwd.h>

int main()
{
    struct stat *info = malloc(sizeof(struct stat));

    stat(NULL, info);
    struct passwd *uid;
    uid = getpwuid(info->st_uid);

}