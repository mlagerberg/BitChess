#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "datatypes.h"
#include "square.h"
#include "board.h"
#include "algebraicnotation.h"
#include "validator.h"


Move *AN_parse(char *str, Board *board) {
	// Generate all moves:
	Move *head = calloc(1,sizeof(Move));
	int color = Board_turn(board);
	int total = v_get_all_valid_moves_for_color(&head, board, color);
	if (total == 0) {
		return NULL;
	}

	int any = false;
	Move *curr = head;
	while (curr) {
		char *move = AN_format(board, curr, false, false);
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

char * AN_format(Board *board, Move *m, int complete, int show_number) {
	Piece *p1 = Board_get_piece(board, m->x, m->y);
	Piece *p2 = Board_get_piece(board, m->xx, m->yy);

	// Move number
	int number = (board->ply_count / 2 + 1);
	// move
	char *move;
	// check or check-mate indication
	char check[2];

	if (p1->shape == KING && abs(m->x - m->xx) > 1) {
		if (m->x < m->xx) {
			move = malloc(4 * sizeof(char));
			sprintf(move, "O-O");
		} else {
			move = malloc(6 * sizeof(char));
			sprintf(move, "O-O-O");
		}
	} else {
		// All those arrays have room for an extra character: \0 to terminate the string.
		// source piece indicator
		char piece[2];
		// source file and/or rank (if needed)
		char src[3];
		// hit
		char hit[2];
		// destination tile
		char *dst = Square_coords_to_string(m->xx, m->yy);
		// promotion
		char prom[3];

		if (p1->symbol[1] == 'p') {
			piece[0] = '\0';
		} else {
			piece[0] = p1->symbol[1];
			piece[1] = '\0';
		}

		// Check for ambiguous moves
		bool hasSource = false;
		if(p1->shape != BISHOP && p1->shape != KING) {
			Square *head = calloc(1,sizeof(Square));
			int count = v_square_attacked_by(&head, board, m->xx, m->yy, -p1->color, p1->shape);
			if(count > 1) {
				int diffFile = true;
				int diffRank = true;
				Square *curr = head;
				while(curr) {
					if(!Square_is_at(curr, m->x, m->y)) {
						if(curr->x == m->x) {
							diffFile = false; // file doesn't differentiate between pieces
						}
						if(curr->y == m->y) {
							diffRank = false; // rank doesn't differentiate between pieces
						}
					}
					curr = curr->next_sibling;
				}
				Square_destroy(head);
				
				hasSource = true;
				if (diffFile) {
					// differentiate by file
					src[0] = m->x + 'a';
					src[1] = '\0';
				} else if(diffRank && p1->shape != PAWN) {
					// differentiate by rank
					// (except pawns - always show file for pawns)
					src[0] = '8' - m->y;
					src[1] = '\0';
				}  else {
					// both are needed to differentiate
					src[0] = m->x + 'a';
					src[1] = '8' - m->y;
					src[2] = '\0';
				}
			}
		}

		// A capture
		if(m->is_en_passant || p2 != NULL) {
			hit[0] = 'x';
			hit[1] = '\0';
			// File must be shown when pawns capture, for some reason
			if (!hasSource && p1->shape == PAWN) {
				hasSource = true;
				src[0] = m->x + 'a';
				src[1] = '\0';
			}
		} else {
			hit[0] = '\0';
		}

		// Promotion
		if(p1->shape == PAWN && (m->yy == 7 || m->yy == 0)) {
			prom[0] = '=';
			switch(m->promotion) {
			case ROOK:		prom[1] = 'R';	break;
			case KNIGHT:	prom[1] = 'N';	break;
			case BISHOP:	prom[1] = 'B';	break;
			case QUEEN:		prom[1] = 'Q';	break;
			}
			prom[2] = '\0';
		} else {
			prom[0] = '\0';
		}

		int len = strlen(piece)
			+ (hasSource ? strlen(src) : 0)
			+ strlen(hit)
			+ strlen(dst)
			+ strlen(prom);
		move = malloc(len * sizeof(char));
		sprintf(move, "%s%s%s%s%s", piece, hasSource ? src : "", hit, dst, prom);
	}

	// Checkmate/Check/Draw
	if(m->gives_check_mate) {
		check[0] = '#';
		check[1] = '\0';
	} else if(m->gives_check) {
		check[0] = '+';
		check[1] = '\0';
	} else {
		check[0] = '\0';
	}

	int len;
	char *out;
	char *format;
	if (show_number) {
		if (complete && p1->color == BLACK) {
			format = "%d. ... %s%s";
			len = 6 + (number >= 10 ? (number >= 100 ? (number >= 1000 ? 4 : 3) : 2) : 1);
		} else {
			format = "%d. %s%s";
			len = 2 + (number >= 10 ? (number >= 100 ? (number >= 1000 ? 4 : 3) : 2) : 1);
		}
	} else {
		format = "%s%s";
		len = 0;
	}
		
	len += strlen(move) + strlen(check);
	out = malloc(len * sizeof(char));
	if (show_number) {
		sprintf(out, format, number, move, check);
	} else {
		sprintf(out, format, move, check);
	}

	if (!m->is_castling) {
		free(move);
	}
	return out;
}


