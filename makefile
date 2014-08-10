# C compiler
CC = gcc

all: executable

# compiler flags:
#	-O4							maximum optimization level I've made it work with
# 	-lpthread					enables multithreading
# 	-finput-charset=UTF-8 		enabled UTF-8 support
# 	-std=c99 -pedantic -ansi	sets a 'clean' C99 mode
# 	-Wall						shows all warnings
ifeq ($(OS),Windows_NT)
    # Windows (without multithreading)
    CFLAGS = -finput-charset=UTF-8 -Wall -O4
else
    # Linux (with multithreading)
    CFLAGS = -lpthread -finput-charset=UTF-8 -Wall -O4
endif

# debugging
debug: CFLAGS += -DDEBUG -g
debug: executable

# output files:
TARGET = chess

# source files:
SOURCE = src/engine/piece.c src/engine/square.c src/engine/board.c src/engine/move.c src/engine/algebraicnotation.c src/engine/simplenotation.c src/engine/validator.c src/engine/fitness.c src/engine/engine.c src/engine/files.c src/debug.c src/tests.c src/main.c


executable: $(SOURCE)
	$(CC) -o$(TARGET) $(SOURCE) $(CFLAGS)
