#!/bin/bash
#
# Builds a debug version, then
# let's the AI do a move on a preconfigured game.
# Helpful when quickly testing changes in the engine
# on a consistent game.

cp testgames/test3 ~/.BitChess/game
make debug
./chess swap
