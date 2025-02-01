# Makefile for structures module

# Compiler and flags
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb

# Files and targets
SRC = structures.c
OBJ = structures.o
LIBCS50 = ../libcs50/libcs50-given.a
SUPPORT = ../support/support.a
HEADER = structures.h

# Default rule
all: $(OBJ)

# Compile the structures.o file
$(OBJ): $(SRC) $(HEADER)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Clean up the build
clean:
	rm -f $(OBJ)

# Phony targets
.PHONY: all clean