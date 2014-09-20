# C compiler
CC = gcc

# source files:
SOURCE = src/debug.c src/main.c src/tests.c src/engine/algebraicnotation.c src/engine/board.c src/engine/engine.c src/engine/files.c src/engine/fitness.c src/engine/heuristics.c src/engine/move.c src/engine/piece.c src/engine/simplenotation.c src/engine/square.c src/engine/validator.c

# output file name:
TARGET = chess

# compiler flags:
#	-On							optimization level
#	-lpthread					enables multithreading
#	-finput-charset=UTF-8		enabled UTF-8 support
#	-std=c99 -pedantic -ansi	sets a 'clean' C99 mode
#	-Wall						shows all warnings
CFLAGS =  -finput-charset=UTF-8 -Wall -O3

# Enable multithreading on Linux
ifneq ($(OS),Windows_NT)
	CFLAGS += -lpthread
endif


all: executable

# debugging flags:
#	-DDEBUG			enables #define DEBUG
#	-g				enables gdb debugging
debug: CFLAGS += -DDEBUG -g
debug: executable

test: executable
	./$(TARGET) test

executable: $(SOURCE)
	$(CC) -o$(TARGET) $(SOURCE) $(CFLAGS)
