# OSN Assignment - Shell

This project is the submission for OSN Assignment 2, 2022.

- Name: Arnav Negi
- Roll: 2021101112
- Branch: CSE

## Prerequisites

 1. GCC compiler
 2. Linux based OS ( built for Ubuntu 20.04)

## Compilation and running

To compile and run the shell, do the following:

 1. Enter the shell directory containing the makefile.
 2. Run make all.
 3. Run ./main

## Files and Directories

The project is built in a modular fashion. The included files are given below:

|File/Directory name |Function or purpose  |
|--|--|
| main.c | Contains the driver code for the shell. Runs most of the basic functions including background processes and running commands from modules. Also initialises some variables.|
|inputs.c|Contains functions to deal with I/O and handle ';' and '&' in commands.|
|builtin|Contains code for the 3 builtin commands for specification 2 - cd (cd.c) pwd (workingdir.c) and echo (echo.c)|
|ls|Contains the code for the ls command for specification 3.|
|pinfo|Contains code for pinfo command for specification 5.|
|discover|Contains code for the discover command for specification 7.|
|history|Contains code for the history command along with the hidden ".history" file storing the command history.|
|helper|Contains helper functions such as prompt, input functions and autocomplete.|
|job_control|Contains functions that handle job control of the shell - fg, bg, jobs and sig.|
|signals|Contains functions handling signals sent by child processes or user keyboard inputs.|
|included.h|Contains all standard c library headers, data declarations and extern variables shared by all files|
|headers.h|Header file containing all include statements required.|

## Assumptions

- Assumptions regarding buffer size for directory names and maximum number of background processes are taken. 200 characters for buffer sizes and 100 background processes at once.
- In command history, the commands "A ;B" and "A ; B" are treated differently due to lack of space between the semicolon and B.
- Running the command "vim &" seems to break the terminal because of unwanted I/O being sent to the driver code. Since all shells don't run "vim &" (they suspend the process immediately) I assume this command won't be required and vim will be used in foreground.
- ~ notation for home directory is not to be used with system programs and only with the programs given above.
