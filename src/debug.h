/**
 * debug.h / debug.c
 *
 * Collection of methods that are useful for printing
 * debug information to the console.
 *
 */
 
#include "engine/datatypes.h"
#include "engine/board.h"
#include "engine/piece.h"
#include "engine/move.h"
#include "engine/validator.h"
#include "engine/engine.h"

#ifndef _DEBUG_H_
#define _DEBUG_H_

/**
 * Generates a semi-random board that's not necessarily valid
 * and is only guaranteed to have both kings on it.
 */
Board *debug_generate_random();

/**
 * Outputs a board to the console, from
 * the viewpoint of the given player.
 * Shows a marker ('XX') at the given field.
 */
void debug_print_mark(Board *b, int player, int x, int y);

/**
 * Outputs a board to the console, from
 * the viewpoint of the given player,
 * with all empty attacked squares showing an XX
 */
void debug_print_check(Board *b, int player);

/**
 * Prints a single coordinate, e.g. "e2\n".
 */
void debug_print_square(int i, int j);

void pause();

#endif
