#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "datatypes.h"
#include "move.h"
#include "board.h"
#include "simplenotation.h"


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

Move *Simple_move_parse(char *buf, Board *board) {
	int color = Board_turn(board);
	int len = strlen(buf);
	// Find castling
	if (strcmp("O-O", buf) == 0 || strcmp("0-0", buf) == 0) {
		if (color == WHITE) {
			return Move_create(color, 4, 7, 6, 7, 0);
		} else {
			return Move_create(color, 4, 0, 6, 0, 0);
		}
	} else if (strcmp("O-O-O", buf) == 0 || strcmp("0-0-0", buf) == 0) {
		if (color == WHITE) {
			return Move_create(color, 4, 7, 2, 7, 0);
		} else {
			return Move_create(color , 4, 0, 2, 0, 0);
		}
	} else if (len == sizeof(char) * 2) {
		// Shorthand form pawn move
		int file2 = buf[0] - 'a';
		int rank2 = 8 - (buf[1] - '0');
		if (file2 < 0 || file2 > 7 || rank2 < 0 || rank2 > 7) {
			fprintf(stderr, "Invalid move: source field is out of bounds.\n");
			return NULL;
		}
		// Find source field
		if (Board_is_at_safe(board, file2, rank2 + color, PAWN, color)) {
			return Move_create(color, file2, rank2 + color, file2, rank2, 0);
		} else if (Board_is_at_safe(board, file2, rank2 + 2 * color, PAWN, color)) {
			return Move_create(color, file2, rank2 + 2 * color, file2, rank2, 0);
		} else {
			fprintf(stderr, "Invalid move: no pawn can move to field %s.\n", buf);
			return NULL;
		}
	} else if (len == sizeof(char) * 3) {
		// Shorthand form normal move, or pawn capture maybe? Or pawn + check ?
		// TODO
	} else if (len > sizeof(char) * 6) {
		fprintf(stderr, "Invalid move: too long (max. 6 characters, like: c7-c8Q).\n");
		return NULL;
	} else {
		int file = buf[0] - 'a';
		int rank = 8 - (buf[1] - '0');
		if (file < 0 || file > 7 || rank < 0 || rank > 7) {
			fprintf(stderr, "Invalid move: source field is out of bounds.\n");
			return NULL;
		}
		int file2 = buf[3] - 'a';
		int rank2 = 8 - (buf[4] - '0');
		if (file2 < 0 || file2 > 7 || rank2 < 0 || rank2 > 7) {
			fprintf(stderr, "Invalid move: destination field is out of bounds.\n");
			return NULL;
		}
		if (file == file2 && rank == rank2) {
			fprintf(stderr, "Invalid move: source is the same as destination.\n");
			return NULL;
		}
		
		int shape = 0;
		// check if buf[5] exist, and parse the piece in it (must be R,N,B or Q)
		if (len == sizeof(char) * 6) {
			switch(buf[5]) {
			case 'R':
				shape = ROOK;	break;
			case 'N':
				shape = KNIGHT;	break;
			case 'B':
				shape = BISHOP;	break;
			case 'Q':
				shape = QUEEN;	break;
			default:
				fprintf(stderr, "Invalid move: invalid promotion piece.\n");
				return NULL;
			}
		}

		return Move_create(color, file, rank, file2, rank2, shape);
	}
	return NULL;
}