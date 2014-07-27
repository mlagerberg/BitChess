echo off
REM c:\MinGW\bin\gcc -ochess src/engine/piece.c src/engine/square.c src/engine/board.c src/engine/move.c src/engine/algebraicnotation.c src/engine/validator.c src/engine/fitness.c src/engine/engine.c src/engine/files.c src/debug.c src/tests.c src/main.c  -lpthread
REM chcp 65001

echo on
c:\MinGW\bin\gcc -ochess src/engine/piece.c src/engine/square.c src/engine/board.c src/engine/move.c src/engine/algebraicnotation.c src/engine/simplenotation.c src/engine/validator.c src/engine/fitness.c src/engine/engine.c src/engine/files.c src/debug.c src/tests.c src/main.c -O3 -finput-charset=UTF-8
