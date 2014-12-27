
#
# `make debug`	enables #define DEBUG in code, resulting in
#				smaller search tree and more verbose logging.
# `make test`	builds the app and runs it's tests.
# `make clean`	removes executable file and src/gitversion.c.
#

# C compiler
CC = gcc

# source files:
SOURCE = src/debug.c src/main.c src/tests.c src/gitversion.c src/engine/algebraicnotation.c src/engine/board.c src/engine/engine.c src/engine/files.c src/engine/fitness.c src/engine/heuristics.c src/engine/move.c src/engine/piece.c src/engine/simplenotation.c src/engine/square.c src/engine/validator.c

# output app name:
TARGET = chess

# compiler flags:
#	-On							optimization level
#	-lpthread					enables multithreading
#	-finput-charset=UTF-8		enabled UTF-8 support
#	-std=c99 -pedantic -ansi	sets a 'clean' C99 mode
#	-Wall						shows all warnings
CFLAGS =  -finput-charset=UTF-8 -Wall -O3

# Platform specific quirks:
# - Linux gets multithreading, Windows doesn't.
# - Windows gets -.exe in the executable filename.
# - Linux echo: quotes, Windows echo: no quotes.
ifeq ($(OS),Windows_NT)
	BINARY = $(TARGET).exe
	Q=
	ESC="
else
	CFLAGS += -lpthread
	BINARY = $(TARGET)
	Q="
	ESC=\"
endif


all: clean executable

.PHONY: clean
clean:
	$(RM) $(BINARY)
	$(RM) src/gitversion.c

src/gitversion.c: .git/HEAD .git/index
	echo $(Q)const char *GIT_VERSION = $(ESC)$(shell git rev-parse --short HEAD)$(ESC);$(Q) > $@

executable: $(SOURCE)
	$(CC) -o$(TARGET) $(SOURCE) $(CFLAGS)

# debugging flags:
#	-DDEBUG			enables #define DEBUG
#	-g				enables gdb debugging
debug: CFLAGS += -DDEBUG -g
debug: executable

test: executable
	./$(TARGET) test

