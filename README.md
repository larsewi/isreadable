# isreadable
A program to check if files are readable while avoiding the possibility of
blocking forever. It works by spawning a thread for each file trying to read 1
Byte. A file is concidered readable if the opening of the file is successfull
and if the reading of 1 Byte is successfull or end-of-file is reached. A file is
not concidered readable if the opening/reading of a file failes, or if a timeout
interval (specified in milliseconds) is reached. The timeout interval protects
you from possibly blocking forever (this is needed until we sove the halting
problem).

## Example
```
$ make                  
gcc -g -Wall -c main.c -o main.o
gcc main.o -o isreadable -pthread
$ ./isreadable -t 100 main.c main.h
main.c is readable.
main.h not readable.
```
