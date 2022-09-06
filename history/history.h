#include "../included.h"

void print_history(sysinfo *currsys);
int getcommands(char **commandlist, sysinfo *currsys);
int addhistory(char *line, sysinfo *currsys);
int comparelines(char *line1, char *line2);