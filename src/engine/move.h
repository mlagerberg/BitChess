#include <stdbool.h>
#include <stdint.h>
#include "datatypes.h"

/**
 * move.h / move.c
 *
 * Functions to create and modify Move instances.
 *
 */
#ifndef _MOVE_H_
#define _MOVE_H_

/**
 * Creates a blank Move; doesn't initialize memory for it,
 * only allocates.
 * Only use then filling in ALL content of the Move yourself,
 * otherwise use Move_create.
 */
Move *Move_alloc();

/**
 * Creates a new Move.
 * Source field is x,y and target is xx,yy. `promotion`, if not
 * EMPTY, is the piece to promote to in case of a pawn promotion.
 */
Move *Move_create(int color, int x, int y, int xx, int yy, int promotion);

/**
 * Copy constructor. Doesn't copy any siblings.
 */
Move *Move_clone(Move *move);

/**
 * Recursively destroys this move and all its following siblings.
 */
void Move_destroy(Move *m);

/**
 * Returns true for moves that are empty dummymoves,
 * that are used as the start of a linked list of moves.
 */
inline bool Move_is_nullmove(Move *m) {
	return (m == NULL)
		|| (m->x == 0 && m->y == 0 && m->xx == 0 && m->yy == 0);
}

inline bool Move_equals(Move *m1, Move *m2) {
	return m1->x == m2->x && m1->y == m2->y && m1->xx == m2->xx && m1->yy == m2->yy;
}

unsigned int Move_get_as_int(Move *m);

/**
 * This is just a simple toString method.
 * For official notation use AN_format(board, move) 
 */
void Move_print(Move *m);

/**
 * Like Move_print(Move*), but with color.
 */
void Move_print_color(Move *m, int color);

void Move_print_all(Move *head);

void Move_print_tree(Move *head);

/**
* Compare values to another move.
* When sorting, this'll sort in order of ascending move value.
* For white this puts the worst move first, for black the best move
*/
inline int Move_compare(Move *m1, Move *m2) {
	return m1->fitness - m2->fitness;
}

/**
 * Constructor for UndoableMoves, i.e. structs that contain instructions on
 * how to undo a move.
 */
UndoableMove *Undo_create(int x, int y, int xx, int yy, int hit_y, Piece *piece, bool white_can_castle_queens_side, bool white_can_castle_kings_side, bool black_can_castle_queens_side, bool black_can_castle_kings_side, bool white_can_en_passant, bool black_can_en_passant);

/**
 * Cleans up an UndoableMove
 */
void Undo_destroy(UndoableMove* umove);

/**
 * Checks if the UndoableMove is the first of a chain of moves.
 * Since the UndoableMoves are basically on a stack (FILO), this
 * technically means the UndoableMove is the last and the Move is the first.
 */
inline bool Move_is_first(UndoableMove *umove) {
	return umove->previous == NULL;
}
	
inline int Move_quiescence(UndoableMove *umove, Board *board) {
	return umove->hit_piece == NULL ? 0 : QUIESCENCE_PENALTY_CAPTURE;
}

#endif