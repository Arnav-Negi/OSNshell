#include "included.h"
char *convert_from_tilde(char *pathname, sysinfo *currsys);
char *convert_to_tilde(char *pathname, sysinfo *currsys);
char *take_input(sysinfo *currsys);
char **tokenize(char *line, char *delim);