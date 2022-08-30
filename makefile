make:
	gcc -o main main.c inputs.c included.h builtin/echo.c builtin/pwd.c builtin/cd.c -Wall -fsanitize=address -Wpedantic -g