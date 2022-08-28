#include "included.h"

extern int errno;
char* home_dir;

#define DEF_BUF_SIZE 200;
#define BUF_SZ_INC 100;

char **parse_input(char *command)
{
    char **args = (char **)malloc(sizeof(char *) * 10);
    strtok(command, " ");
}

char *take_input()
{
    // Take input and handle buffer OF. Parse multiple inputs.
    int BUF_SIZE = DEF_BUF_SIZE;
    int ptr = 0;
    char c;
    char *input = (char *)malloc(sizeof(char) * BUF_SIZE);

    if (input == NULL)
    {
        perror("out of memory while taking input.\n");
        exit(1);
    }

    while (1)
    {
        scanf("%c", &c);

        if (c == EOF || c == '\n')
        {
            input[ptr] = '\0';
            return input;
        }
        else
        {
            input[ptr++] = c;
        }

        // dymnamic memory alloc
        if (ptr >= BUF_SIZE)
        {
            BUF_SIZE += BUF_SZ_INC;
            input = realloc(input, BUF_SIZE);

            if (input == NULL)
            {
                perror("out of memory while taking input.\n");
                exit(1);
            }
        }
    }
}

void shell_loop()
{
    char *line, *command;
    char **args;
    int status;

    do
    {
        line = take_input();
        
        command = strtok(line, ";&");

        free(line), free(command), free(args);
    } while (status);
}

int main(int argc, char **argv)
{ 
    // set home directory.

    shell_loop();

    return 0;
}