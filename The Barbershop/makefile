#Kai Shi, Wen-yin Wang, Xu Zheng
#CS544-001
#Concurrency #5

CC = gcc
DEBUG = -g
#CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
OSUFFIX = -lrt -pthread
OBJ1 = concurrency5_1.o
PROG1 = concurrency5_1
SRC1 = concurrency5_1.c
OBJ2 = concurrency5_2.o
SRC2 = concurrency5_2.c
PROG2 = concurrency5_2

all: $(PROG1) $(PROG2)
	rm -f makefile~ *.c~

$(PROG1): $(OBJ1) #$(OBJ2)
	$(CC) -o $@ $^ $(OSUFFIX)

$(PROG2): $(OBJ2)
	$(CC) -o $@ $^ $(OSUFFIX)

$(OBJ1): $(SRC1)
	$(CC) $(CFLAGS) -c $<

$(OBJ2): $(SRC2)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f makefile~ *.o *.c~ $(PROG1) $(PROG2)