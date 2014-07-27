#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "engine/common.h"
#include "engine/datatypes.h"
#include "engine/board.h"
#include "engine/piece.h"
#include "engine/move.h"
#include "engine/validator.h"
#include "engine/engine.h"
#include "debug.h"


void pause() {
	printf("Press enter to continue...\n");
	getchar();
}

Board *debug_generate_random() {
	Board *b = calloc(1,sizeof(Board));
	int i,j;
	Piece *piece;
	// Init randomizer:
	srand(time(NULL));

	b->ply_count = rand() % 20 + 5;
	b->fifty_move_count = rand() % b->ply_count + 5;
	b->white_can_castle_kings_side = true;
	b->white_can_castle_queens_side = true;
	b->black_can_castle_kings_side = true;
	b->black_can_castle_queens_side = true;
	b->white_can_en_passant = -1;
	b->black_can_en_passant = -1;	
	for(i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			b->fields[i][j] = NULL;
		}
	}

	// File the board with random pieces:
	int x, y, shape, color;
	for(i = 0; i < 5; i++) {
		x = rand() % 8;
		y = rand() % 8;
		shape = rand() % 6;
		color = (rand() % 10 > 5 ? WHITE : BLACK);
		piece = Piece_create(shape, color);
		Board_set(b, x, y, piece);
		//printf("Put %s at ", piece->symbol);
		//debug_print_square(x, y);
	}
	x = rand() % 8;
	y = rand() % 8;
	Board_set(b, x, y, Piece_create(KING, WHITE));
	//printf("Put white King at ");
	//debug_print_square(x, y);
	while (Board_is_at(b, x, y, KING, WHITE)) {
		x = rand() % 8;
		y = rand() % 8;
	}
	Board_set(b, x, y, Piece_create(KING, BLACK));
	//printf("Put black King at ");
	//debug_print_square(x, y);
	return b;
}

void debug_print_mark(Board *b, int player, int x, int y) {
	int i,j,row,col;
	if (player == BLACK) {
		printf("   H  G  F  E  D  C  B  A\n");
	}
	printf("  +--+--+--+--+--+--+--+--+\n");
	for (i = 0; i < 8; i++) {
		if (i > 0) {
			printf("  +--+--+--+--+--+--+--+--+\n");
		}
		if (player == WHITE) {
			printf("%c ", '8' - i);
		} else {
			printf("  ");
		}
		for (j = 0; j < 8; j++) {
			printf("|");
			if (player == WHITE) {
				row = i; col = j;
			} else {
				row = 7-i; col = 7-j;
			}
			if (col == x && row == y) {
				printf("XX");
			} else if (b->fields[col][row] == NULL) {
				printf("  ");
			} else {
				char *name = Piece_short_name(b->fields[col][row]);
				printf("%s", name);
			}			
		}
		if (player == BLACK) {
			printf("| %c\n", '1' + i);
		} else {
			printf("|\n");
		}
	}
	printf("  +--+--+--+--+--+--+--+--+\n");
	if (player == WHITE) {
		printf("   A  B  C  D  E  F  G  H\n");
	}
}

void debug_print_check(Board *b, int player) {
	int i,j,row,col;
	if (player == BLACK) {
		printf("   H  G  F  E  D  C  B  A\n");
	}
	printf("  +--+--+--+--+--+--+--+--+\n");
	for (i = 0; i < 8; i++) {
		if (i > 0) {
			printf("  +--+--+--+--+--+--+--+--+\n");
		}
		if (player == WHITE) {
			printf("%c ", '8' - i);
		} else {
			printf("  ");
		}
		for (j = 0; j < 8; j++) {
			printf("|");
			if (player == WHITE) {
				row = i; col = j;
			} else {
				row = 7-i; col = 7-j;
			}
			if (b->fields[col][row] == NULL) {
				if (v_square_gives_check(b, col, row, player)) {
					printf("XX");
				} else {
					printf("  ");
				}
			} else {
				char *name = Piece_short_name(b->fields[col][row]);
				printf("%s", name);
			}			
		}
		if (player == BLACK) {
			printf("| %c\n", '1' + i);
		} else {
			printf("|\n");
		}
	}
	printf("  +--+--+--+--+--+--+--+--+\n");
	if (player == WHITE) {
		printf("   A  B  C  D  E  F  G  H\n");
	}
}

void debug_print_square(int i, int j) {
	printf("%c%d\n", i + 97, 8 - j);
}