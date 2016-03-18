CC = gcc
CFLAGS = -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement

SOURCES1 = hw1.c
OBJECTS1 = $(SOURCES1:.c=.o)
PROGS1 = hw1

all: $(SOURCES1) $(PROGS1)

$(PROGS1): $(OBJECTS1)
	$(CC) $(CFLAGS) -g -o $(PROGS1) $(OBJECTS1) -pthread

hw1.o: $(SOURCES1)
	$(CC) -c $(CFLAGS) $(SOURCES1)

clean:
	rm -f *.o *.c~ $(PROGS1)