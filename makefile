
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
#	-finput-charset=UTF-8		enabled UTF-8 support (on Linux and Windows)
#	-std=c99 -pedantic -ansi	sets a 'clean' C99 mode
#	-Wall						shows all warnings
CFLAGS =  -Wall -O3

# Platform specific quirks:
# - Unix gets multithreading, Windows doesn't.
# - Mac doesn't need or understand the UTF-8 flag
# - Windows gets -.exe in the executable filename.
# - Unix echo: quotes, Windows echo: no quotes.
OS := $(shell uname)
ifeq ($(OS),windows32)
	CFLAGS += -finput-charset=UTF-8
	BINARY = $(TARGET).exe
	Q=
	ESC="
else ifeq ($(OS),Darwin)
	CFLAGS += -lpthread
	BINARY = $(TARGET)
	Q="
	ESC=\"
else
	CFLAGS += -finput-charset=UTF-8 -lpthread
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

