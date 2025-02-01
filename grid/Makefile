# Makefile for grid (temp)
#


OBJS = gridtest.o grid.o ../structures/structures.o
LIBS = ../libcs50/libcs50-given.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../lib
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

gridtest: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

gridtest.o: grid.h
grid.o: grid.c ../libcs50/file.h ../libcs50/mem.h ../structures/structures.h

# Compile structures.c to structures.o
structures.o: ../structures/structures.h ../libcs50/set.h ../libcs50/mem.h
	$(CC) $(CFLAGS) -c ../structures/structures.c -o ../structures/structures.o

.PHONY: test valgrind clean

all: gridtest

test: gridtest
	./gridtest

valgrind: gridtest
	$(VALGRIND) ./gridtest

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f gridtest
	rm -f core