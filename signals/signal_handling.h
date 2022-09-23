#include "../included.h"

void ctrlZ_handler(int signum);
void ctrlC_handler(int signum);
void ctrlD_handler(int signum);
void rembg(int signum);
int newbg(int pid, char *proccess_name, char *command);