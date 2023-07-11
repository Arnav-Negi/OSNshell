#pragma once

#include "../included.h"
#include "../signals/signal_handling.h"
#include "autosuggest.h"

char *convert_from_tilde(char *pathname, sysinfo *currsys);
char *convert_to_tilde(char *pathname, sysinfo *currsys);
char **take_input(sysinfo *currsys);
char **tokenize(char *line, char *delim);
int changeIO(int inputfile, int outputfile);
int parseIO(int argc, char **args);
void disableRawMode();
void enableRawMode();