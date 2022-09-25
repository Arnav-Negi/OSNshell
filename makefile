DEPS := main.o history/history.o pinfo/pinfo.o \
				builtin/echo.o builtin/workingdir.o builtin/cd.o \
				discover/discover.o ls/ls.o job_control/jobs.o \
				job_control/signal.o job_control/fg.o \
				job_control/bg.o signals/signal_handling.o \
				helper/prompt.o helper/inputs.o helper/autosuggest.o
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