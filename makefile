make:
	gcc -o main main.c inputs.c included.h builtin/echo.c builtin/workingdir.c builtin/cd.c ls/ls.c -Wall -fsanitize=address -Wpedantic -g