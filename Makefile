CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -pthread

.PHONY: all clean

all: isreadable

isreadable: main.o
	$(CC) $^ -o $@ $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f isreadable
