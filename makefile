make:
	gcc -o main main.c inputs.c included.h history/history.c pinfo/pinfo.c builtin/echo.c builtin/workingdir.c builtin/cd.c discover/discover.c ls/ls.c -Wall -fsanitize=address -Wpedantic -g