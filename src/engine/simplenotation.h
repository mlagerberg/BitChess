#include "common.h"
#include "datatypes.h"
#include "move.h"

/**
 * simplenotation.h / simplenotation.c
 *
 * Contains functions to read and write a simplified notation.
 * One that does not allow for symbols like + and !, that requires
 * the source field to be included (so no `e4` as short for `e2-e4`),
 *
 * This simplified notation is used when adding `-m` as a command line
 * argument, and can be used when writing a script that 
 * communicates with the program without having to write
 * smart parsing and formatting code.
 */
#ifndef _SIMPLENOTATION_H_
#define _SIMPLENOTATION_H_


/**
 * Returns the simplified notation of the given move.
 */
char * Simple_move_format(Board *board, Move *m);


/**
* Parses an string representation of a move into an actual move. Note that
* shorthand notations are not accepted and neither are symbols like +,!
* To castle, enter O-O or O-O-O (with capital 'Oh').
* Pawns are automatically promoted to a Queen, unless a capital 'N'
* is appended to the move. This method DOES NOT check if a move is valid,
* only that the pieces are still on the board.
*/
Move * Simple_move_parse(char *str, Board *board);

#endif
