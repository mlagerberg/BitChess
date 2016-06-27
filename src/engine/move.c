#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "color.h"
#include "common.h"
#include "datatypes.h"
#include "move.h"
#include "piece.h"

Move *Move_alloc() {
	return calloc(1,sizeof(Move));
}

Move *Move_create(int color, int x, int y, int xx, int yy, int promotion) {
	Move *m = Move_alloc();
	m->x = x; m->y = y;
	m->xx = xx; m->yy = yy;
	m->is_castling = false;
	m->is_en_passant = false;
	m->gives_check = false;
	m->gives_draw = false;
	m->gives_check_mate = false;
	m->is_evasion = false;
	m->promotion = promotion;
	m->fitness = (color == WHITE ? MIN_FITNESS : MAX_FITNESS);
	m->next_sibling = NULL;
	return m;
}

Move *Move_clone(Move *move) {
	Move *clone = Move_create(0, move->x, move->y, move->xx, move->yy, move->promotion);
	clone->fitness = move->fitness;
	clone->is_castling = move->is_castling;
	clone->is_en_passant = move->is_en_passant;
	clone->gives_check = move->gives_check;
	clone->gives_draw = move->gives_draw;
	clone->gives_check_mate = move->gives_check_mate;
	clone->is_evasion = move->is_evasion;
	return clone;
}

void Move_destroy(Move *m) {
	if (m != NULL && m->next_sibling != NULL) {
		Move_destroy(m->next_sibling);
	}
	free(m);
}

extern inline bool Move_is_nullmove(Move *m);

extern inline bool Move_equals(Move *m1, Move *m2);

extern inline int Move_compare(Move *m1, Move *m2);

unsigned int Move_get_as_int(Move *m) {
	unsigned int r = 0;
	r |= (((char) m->x) << 24);
	r |= (((char) m->y) << 16);
	r |= (((char) m->x) << 8);
	r |= (char) m->x;
	return r;
}

void Move_print(Move *m) {
	if (m->promotion == 0) {
		printf("%c%d-%c%d", m->x + 'a', 8 - m->y, m->xx + 'a', 8 - m->yy);
	} else {
		char prom;
		switch(m->promotion) {
		case ROOK:		prom = 'R';	break;
		case KNIGHT:	prom = 'N';	break;
		case BISHOP:	prom = 'B';	break;
		default:
		case QUEEN:		prom = 'Q';	break;
		}
		printf("%c%d-%c%d%c", m->x + 'a', 8 - m->y, m->xx + 'a', 8 - m->yy, prom);
	}
}

void Move_print_color(Move *m, int color) {
	printf(color == WHITE ? color_white : color_black);
	Move_print(m);
	printf(resetcolor);
}

void Move_print_all(Move *head) {
	Move *curr;
	curr = head;
	while(curr) {
		printf("%c%d-%c%d, value: %d\n", curr->x + 'a', 8 - curr->y, curr->xx + 'a', 8 - curr->yy, curr->fitness);
		curr = curr->next_sibling;
	}
}

UndoableMove *Undo_create(int x, int y, int xx, int yy, int hit_y, Piece *piece, bool white_can_castle_queens_side, bool white_can_castle_kings_side, bool black_can_castle_queens_side, bool black_can_castle_kings_side, bool white_can_en_passant, bool black_can_en_passant) {
	UndoableMove *umove = malloc(sizeof(UndoableMove));
	umove->x = x; umove->y = y;
	umove->xx = xx; umove->yy = yy;
	umove->hit_y = (hit_y == -1 ? y : hit_y);
	umove->hit_piece = piece;
	umove->white_can_castle_queens_side = white_can_castle_queens_side;
	umove->white_can_castle_kings_side = white_can_castle_kings_side;
	umove->black_can_castle_queens_side = black_can_castle_queens_side;
	umove->black_can_castle_kings_side = black_can_castle_kings_side;
	umove->white_can_en_passant = white_can_en_passant;
	umove->black_can_en_passant = black_can_en_passant;
	umove->is_promotion = false;
	umove->is_castling = false;
	umove->adds_to_fifty = false;
	umove->previous = NULL;
	return umove;
}

void Undo_destroy(UndoableMove* umove) {
	free(umove);
}

extern inline bool Move_is_first(UndoableMove *umove);

extern inline int Move_quiescence(UndoableMove *umove, Board *board);
