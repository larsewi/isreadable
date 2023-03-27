CC = gcc
CFLAGS = -g -Wall -Wextra
LDFLAGS = -pthread

.PHONY: all format clean

all: readable

readable: main.o readable.o
	$(CC) $^ -o $@ $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

readable.o: readable.c
	$(CC) $(CFLAGS) -Wno-int-to-void-pointer-cast -Wno-void-pointer-to-int-cast -c $< -o $@

format:
	clang-format -verbose -i *.c *.h

clean:
	rm -f *.o
	rm -f readable
