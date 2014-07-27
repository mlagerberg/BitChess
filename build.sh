#! /bin/bash

# This script builds the chess engine
# using a simple gcc command and some flags.
# The output binary is called `chess`.

OUTPUT=-ochess
FILES=(src/engine/piece.c src/engine/square.c src/engine/board.c src/engine/move.c src/engine/algebraicnotation.c src/engine/simplenotation.c src/engine/validator.c src/engine/fitness.c src/engine/engine.c src/engine/files.c src/debug.c src/tests.c src/main.c)
FLAGS=(-lpthread -finput-charset=UTF-8 -Wall -O4)
# Explanation:
#	-O4							maximum optimization level I've made it work with
# 	-lpthread					enables multithreading
# 	-finput-charset=UTF-8 		enabled UTF-8 support
# 	-std=c99 -pedantic -ansi	sets a 'clean' C99 mode
# 	-Wall						shows all warnings

gcc ${OUTPUT} ${FILES[@]} ${FLAGS[@]}
