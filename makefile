DEPS := main.o inputs.o history/history.o pinfo/pinfo.o \
				builtin/echo.o builtin/workingdir.o builtin/cd.o \
				discover/discover.o ls/ls.o
CC := gcc
CFLAGS := -o main -Wall -fsanitize=address -Wpedantic -g

$(DEPS): %.o: %.c

.PHONY: clean

make all:
	make build; make clean

build:	$(DEPS)
	$(CC) -o main $(DEPS) $(CFLAGS)

clean:
	rm -f **/*.o *.o *.out

del: 
	rm -f main