#include "board.h"
#include "stats.h"

/**
 * engine.h / engine.c
 *
 * The core of the chess engine, containing the alpha-beta algorithm
 * and various functions around it.
 * Most functions are static and declared in engine.c itself,
 * only Engine_turn, the method that makes the engine calculate one
 * single turn, is to be called from outside engine.c.
 */
#ifndef _ENGINE_H_
#define _ENGINE_H_


/**
 * Thinks of the best move for the AI's turn (given color).
 * Uses get_best_move to generate a list of best moves, and
 * (if enabled) uses dynamic ply depth to think extra hard about
 * the best few moves.
 *
 * Returns the 'best' move for the given color.
 */
Move *Engine_turn(Board *board, Stats *stats, int color, int ply_depth, int verbosity);

#endif