#include "../included.h"

void ctrlZ_handler(int signum);
void DoNothing(int signum);
void rembg(int signum);
int newbg(int pid, char *proccess_name, char *command, int status);