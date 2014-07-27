#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "datatypes.h"
#include "piece.h"
#include "board.h"
#include "move.h"

Move *Move_create(int color, int x, int y, int xx, int yy, int promotion) {
	Move *m = malloc(sizeof(Move));
	
	m->x = x; m->y = y;
	m->xx = xx; m->yy = yy;
	m->is_castling = false;
	m->is_en_passant = false;
	m->gives_check = false;
	m->gives_draw = false;
	m->gives_check_mate = false;
	m->promotion = promotion;
	m->fitness = (color == WHITE ? MIN_FITNESS : MAX_FITNESS);
	m->heuristic = 0;
	m->next_sibling = NULL;
	return m;
}

Move *Move_clone(Move *move) {
	Move *clone = Move_create(0, move->x, move->y, move->xx, move->yy, move->promotion);
	clone->fitness = move->fitness;
	clone->heuristic = move->heuristic;
	clone->is_castling = move->is_castling;
	clone->is_en_passant = move->is_en_passant;
	clone->gives_check = move->gives_check;
	clone->gives_draw = move->gives_draw;
	clone->gives_check_mate = move->gives_check_mate;
	return clone;
}

void Move_destroy(Move *m) {
	if (m == NULL) {
		return;
	}
	if (m->next_sibling != NULL) {
		Move_destroy(m->next_sibling);
	}
	free(m);
}

bool Move_is_nullmove(Move *m) {
	return (m == NULL)
		|| (m->x == 0 && m->y == 0 && m->xx == 0 && m->yy == 0);
}

bool Move_equals(Move *m1, Move *m2) {
	return m1->x == m2->x && m1->y == m2->y && m1->xx == m2->xx && m1->yy == m2->yy;
}

int Move_compare(Move *m1, Move *m2) {
	return m1->fitness - m2->fitness;
}

void Move_print(Move *m) {
	if (m->promotion == 0) {
		printf("%c%d-%c%d\n", m->x + 'a', 8 - m->y, m->xx + 'a', 8 - m->yy);
	} else {
		char prom;
		switch(m->promotion) {
		case ROOK:		prom = 'R';	break;
		case KNIGHT:	prom = 'N';	break;
		case BISHOP:	prom = 'B';	break;
		default:
		case QUEEN:		prom = 'Q';	break;
		}
		printf("%c%d-%c%d%c\n", m->x + 'a', 8 - m->y, m->xx + 'a', 8 - m->yy, prom);
	}
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

bool Move_is_first(UndoableMove *umove) {
	return umove->previous == NULL;
}

int Move_quiescence(UndoableMove *umove, Board *board) {
	return umove->hit_piece == NULL ? 0 : QUIESCENCE_PENALTY_CAPTURE;
}
