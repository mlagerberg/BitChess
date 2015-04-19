#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "datatypes.h"
#include "move.h"
#include "board.h"
#include "simplenotation.h"
#include "validator.h"


char * Simple_move_format(Board *board, Move *m) {
	char *move;
	if (m->promotion == 0) {
		move = malloc(6 * sizeof(char));
		sprintf(move, "%c%d-%c%d", m->x + 'a', 8 - m->y, m->xx + 'a', 8 - m->yy);
	} else {
		char prom;
		switch(m->promotion) {
		case ROOK:		prom = 'R';	break;
		case KNIGHT:	prom = 'N';	break;
		case BISHOP:	prom = 'B';	break;
		default:
		case QUEEN:		prom = 'Q';	break;
		}
		move = malloc(7 * sizeof(char));
		sprintf(move, "%c%d-%c%d%c", m->x + 'a', 8 - m->y, m->xx + 'a', 8 - m->yy, prom);
	}
	return move;
}

Move *Simple_move_parse(char *str, Board *board) {
	// Generate all moves:
	Move *head = Move_alloc();
	int color = Board_turn(board);
	int total = v_get_all_valid_moves_for_color(&head, board, color);
	if (total == 0) {
		Move_destroy(head);
		return NULL;
	}

	int any = false;
	Move *curr = head;
	while (curr) {
		char *move = Simple_move_format(board, curr);
		if (strcmp(str, move) == 0) {
			any = true;
			free(move);
			break;
		}
		free(move);
		curr = curr->next_sibling;
	}
	// Make a copy, so the rest can easily be destroyed in 1 go:
	if (any) {
		Move *result = Move_clone(curr);
		Move_destroy(head);
		return result;
	}

	return NULL;
}
