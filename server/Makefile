# Makefile for server

OBJS = server.o ../structures/structures.o ../vision/vision.o ../grid/grid.o
LIBS = ../libcs50/libcs50-given.a ../support/support.a -lm

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../lib
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

server: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
server.o: server.c ../libcs50/mem.h ../support/message.h ../libcs50/set.h ../grid/grid.h ../vision/vision.h ../structures/structures.h
grid.o: ../grid/grid.c ../libcs50/file.h ../libcs50/mem.h ../structures/structures.c
structures.o: ../libcs50/set.h ../libcs50/mem.h ../support/message.h ../structures/structures.h
vision.o: ../structures/structures.h ../grid/grid.h ../libcs50/set.h ../libcs50/mem.h ../libcs50/file.h ../vision/vision.h

.PHONY: test valgrind clean

all: server


valgrind: server
	$(VALGRIND) ./server

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f server
	rm -f core