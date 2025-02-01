# Makefile for vision module 
#
#

# Compiler and flags
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb

# Target executable
TARGET = visiontest

# Source files and object files
SRCS = visiontest.c ../vision/vision.c ../structures/structures.c ../grid/grid.c
OBJS = visiontest.o vision.o ../structures/structures.o ../grid/grid.o

# Libraries
LIBS = ../libcs50/libcs50-given.a ../support/support.a

# Header files
HDRS = vision.h ../structures/structures.h ../grid/grid.h ../libcs50/set.h ../libcs50/mem.h ../libcs50/file.h

# Default target
all: $(TARGET)

# Build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TARGET) -lm

# Compile each source file into an object file
# Note: in vision test: w,a,s,t keys move the player around the main grid (press enter each time)
visiontest.o: visiontest.c $(HDRS)
	$(CC) $(CFLAGS) -c visiontest.c -o visiontest.o

vision.o: ../vision/vision.c $(HDRS)
	$(CC) $(CFLAGS) -c ../vision/vision.c -o vision.o

../structures/structures.o:
	$(MAKE) -C ../structures

../grid/grid.o:
	$(MAKE) -C ../grid

# Clean up build files
clean:
	rm -f *.o $(TARGET)