#include <stdbool.h>
#include "board.h"
#include "move.h"

/**
 * validator.h / validator.c
 *
 * The methods in this file generate valid moves for a given
 * board position, and is able to verify if a given move is valid.
 *
 * 'Public' functions in this file all start with the prefix `v_`.
 * Other, static functions are declared in validator.c
 *
 */

#ifndef _VALIDATOR_H_
#define _VALIDATOR_H_


bool v_is_valid_move(Board *board, Move *move);

/**
 * Returns the number of valid moves, and puts the first
 * of those moves in the first parameter.
 *
 * The first parameter must be allocated like so:
 * Move *head = calloc(1,sizeof(Move));
 */
int v_get_all_valid_moves_for_color(Move **head, Board *board, int color);

/**
 * Returns true if the King of given color is at check
 */
bool v_king_at_check(Board *board, int color);

/**
 * Returns the number of valid moves for the piece at the given field.
 */
int v_get_valid_move_count_for_piece(Board *board, int x, int y);

/**
 * Returns false if the given square is save for the King to stand on.
 * Used for Castling, where the King is not allowed to pass fields that
 * would check, and by algebraicnotation.c to check if a move gives check.
 */
bool v_square_gives_check(Board *board, int x, int y, int color);

/**
 * Returns a list of attacking pieces that attack the given square.
 * Used by algebraicnotation.c to check if a certain move notation
 * is ambiguous.
 * head  - first output square
 * x 	 - the file of the square
 * y 	 - the rank of the square
 * color - the of the ATTACKED player
 * shape - the type of pieces to look for
 * Attaches a list to {head}, of Squares occupied by pieces of the opponent
 * (-color) that attack the specified square.
 * Returns the number of squares added to the list.
 */
int v_square_attacked_by(Square **head, Board *board, int x, int y, int color, int shape);

#endif