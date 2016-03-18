CC = gcc
CFLAGS = -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
SOURCES1 = concurrency4.c
OBJECTS1 = $(SOURCES1:.c=.o)
PROGS1 = concurrency4



all: $(SOURCES1) $(PROGS1) 

$(PROGS1): $(OBJECTS1)
	$(CC) $(CFLAGS) -o $(PROGS1) $(OBJECTS1) -pthread

 concurrency4.o: $(SOURCES1)
	$(CC) -c $(CFLAGS) $(SOURCES1) -pthread




clean:
	rm -f *.o *.c~ $(PROGS1) $(PROGS2) $(PROGS3)
