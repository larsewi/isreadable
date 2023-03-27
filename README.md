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
% make
gcc -g -Wall -Wextra -c main.c -o main.o
gcc -g -Wall -Wextra -Wno-int-to-void-pointer-cast -Wno-void-pointer-to-int-cast -c readable.c -o readable.o
gcc main.o readable.o -o readable -pthread
$ ./readable -t100 *.c *.h not-a-file
File 'main.c' is readable.
File 'readable.c' is readable.
File 'readable.h' is readable.
File 'not-a-file' is not readable.
```
