make:
	gcc -o main main.c inputs.c included.h echo.c pwd.c -Wall -fsanitize=address -Wpedantic -g