#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "color.h"
#include "common.h"
#include "datatypes.h"
#include "fitness.h"
#include "move.h"
#include "piece.h"

/**
 * Reads to chars from the file into the buffer,
 * returns false if EOF is reached.
 */
static int read_two_chars(FILE *file, char *buf);

Board *Board_create() {
	Board *b = malloc(sizeof(Board));
	Board_reset(b);
	return b;
}

Board *Board_clone(Board *src) {
	Board *b = malloc(sizeof(Board));
	int i,j;
	for(i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (src->fields[i][j] == NULL) {
				b->fields[i][j] = NULL;
			} else {
				b->fields[i][j] = Piece_clone(src->fields[i][j]);
			}
		}
	}
	b->ply_count = src->ply_count;
	b->fifty_move_count = src->fifty_move_count;

	b->white_can_castle_kings_side = src->white_can_castle_kings_side;
	b->white_can_castle_queens_side = src->white_can_castle_queens_side;
	b->black_can_castle_kings_side = src->black_can_castle_kings_side;
	b->black_can_castle_queens_side = src->black_can_castle_queens_side;
	b->white_can_en_passant = src->white_can_en_passant;
	b->black_can_en_passant = src->black_can_en_passant;
	
	b->state = src->state;

	b->captures_white_count = 0;
	b->captures_black_count = 0;
	b->captures_white = NULL;
	b->captures_black = NULL;
	return b;
}

void Board_destroy(Board *b) {
	int i,j;
	for (i = 0; i < 8; i++) {
		for(j = 0; j < 8; j++) {
			if (b->fields[i][j] != NULL) {
				Piece_destroy(b->fields[i][j]);
			}
		}
	}
	if (b->captures_white) {
		Capture_destroy(b->captures_white);
	}
	if (b->captures_black) {
		Capture_destroy(b->captures_black);
	}
	free(b);
}

void Board_reset(Board *b) {
	int i,j;

	b->ply_count = 0;
	b->fifty_move_count = 0;

	b->white_can_castle_kings_side = true;
	b->white_can_castle_queens_side = true;
	b->black_can_castle_kings_side = true;
	b->black_can_castle_queens_side = true;
	b->white_can_en_passant = -1;
	b->black_can_en_passant = -1;

	b->state = UNFINISHED;

	b->captures_white_count = 0;
	b->captures_black_count = 0;
	b->captures_white = NULL;
	b->captures_black = NULL;

	for(i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			int shape = EMPTY;
			if (j == 1 || j == 6) {
				shape = PAWN;
			} else if (i == 0 || i == 7) {
				shape = ROOK;
			} else if (i == 1 || i == 6) {
				shape = KNIGHT;
			} else if (i == 2 || i == 5) {
				shape = BISHOP;
			} else if (i == 3) {
				shape = QUEEN;
			} else {
				shape = KING;
			}
			if (j <= 1) {
				b->fields[i][j] = Piece_create(shape, BLACK);
			} else if (j >= 6) {
				b->fields[i][j] = Piece_create(shape, WHITE);
			} else if (b->fields[i][j] != NULL) {
				//Piece_destroy(b->fields[i][j]);
				b->fields[i][j] = NULL;
			}
		}
	}
}

#ifdef UNICODE_OUTPUT
void Board_print(Board *b, int player) {
	int i,j,row,col;
	if (player == WHITE) {
		printf("    A   B   C   D   E   F   G   H\n");
	} else {
		printf("    H   G   F   E   D   C   B   A\n");
	}
	printf("  ╔═══╤═══╤═══╤═══╤═══╤═══╤═══╤═══╗\n");
	for (i = 0; i < 8; i++) {
		if (i > 0) {
			printf("  ╟───┼───┼───┼───┼───┼───┼───┼───╢ \n");
		}
		if (player == WHITE) {
			printf("%c", '8' - i);
		} else {
			printf("%c", '1' + i);
		}
		for (j = 0; j < 8; j++) {
			printf(j == 0 ? " ║ " : " | ");
			if (player == WHITE) {
				row = i; col = j;
			} else {
				row = 7-i; col = 7-j;
			}
			Piece_print(b->fields[col][row]);
		}
		if (player == BLACK) {
			printf(" ║ %c  ", '1' + i);
		} else {
			printf(" ║ %c  ", '8' - i);
		}
		if (i == 0) {
			Board_print_captures(b, -player);
		} else if (i == 7) {
			Board_print_captures(b, player);
		}
		printf("\n");
	}
	printf("  ╚═══╧═══╧═══╧═══╧═══╧═══╧═══╧═══╝\n");
	if (player == WHITE) {
		printf("    A   B   C   D   E   F   G   H\n");
	} else {
		printf("    H   G   F   E   D   C   B   A\n");
	}

	if (b->state != UNFINISHED) {
		switch(b->state) {
		case WHITE_WINS:
			printf("\nWhite wins!\n");
			break;
		case BLACK_WINS:
			printf("\nBlack wins!\n");
			break;
		case DRAW:
			printf("\nGame ended in a draw.\n");
			break;
		case STALE_MATE:
			printf("\nStale mate!\n");
			break;
		}
	}
}

void Board_print_color(Board *b, int player) {
	int i,j,row,col;
	char *tile1, *tile2;
	if (player == WHITE) {
		printf("     A    B    C    D    E    F    G    H\n");
		tile1 = TILE_WHITE;
		tile2 = TILE_BLACK;
	} else {
		printf("     H    G    F    E    D    C    B    A\n");
		tile2 = TILE_WHITE;
		tile1 = TILE_BLACK;
	}
	printf("   %s     %s     %s     %s     %s     %s     %s     %s     %s\n"
			, tile1,tile2,tile1,tile2,tile1,tile2,tile1,tile2,resetcolor);
	for (i = 0; i < 8; i++) {
		if (i > 0) {
			printf("   %s     %s     %s     %s     %s     %s     %s     %s     %s \n"
					, tile1,tile2,tile1,tile2,tile1,tile2,tile1,tile2,resetcolor);
		}
		if (player == WHITE) {
			printf("%c", '8' - i);
		} else {
			printf("%c", '1' + i);
		}
		for (j = 0; j < 8; j++) {
			printf("  %s  ", j % 2 == 0 ? tile1 : tile2);
			if (player == WHITE) {
				row = i; col = j;
			} else {
				row = 7-i; col = 7-j;
			}
			Piece_print_color(b->fields[col][row]);
		}
		if (player == BLACK) {
			printf("  %s  %c  ", resetcolor, '1' + i);
		} else {
			printf("  %s  %c  ", resetcolor, '8' - i);
		}
		if (i == 0) {
			Board_print_captures(b, -player);
		} else if (i == 7) {
			Board_print_captures(b, player);
		}
		printf("\n");
		printf("   %s     %s     %s     %s     %s     %s     %s     %s     %s \n"
					, tile1,tile2,tile1,tile2,tile1,tile2,tile1,tile2,resetcolor);
		char *t = tile2;
		tile2 = tile1;
		tile1 = t;
	}
	if (player == WHITE) {
		printf("     A    B    C    D    E    F    G    H\n");
	} else {
		printf("     H    G    F    E    D    C    B    A\n");
	}

	if (b->state != UNFINISHED) {
		switch(b->state) {
		case WHITE_WINS:
			printf("\nWhite wins!\n");
			break;
		case BLACK_WINS:
			printf("\nBlack wins!\n");
			break;
		case DRAW:
			printf("\nGame ended in a draw.\n");
			break;
		case STALE_MATE:
			printf("\nStale mate!\n");
			break;
		}
	}
}

void Board_print_captures(Board *b, int color) {
	if (color == WHITE) {
		Capture *curr = b->captures_white;
		while(curr) {
			Piece_print(curr->piece);
			printf(" ");
			curr = curr->next_sibling;
		}
	} else if (color == BLACK) {
		Capture *curr = b->captures_black;
		while(curr) {
			Piece_print(curr->piece);
			printf(" ");
			curr = curr->next_sibling;
		}
	}
}
#else
void Board_print(Board *b, int player) {
	int i,j,row,col;
	if (player == WHITE) {
		printf("    A   B   C   D   E   F   G   H\n");
	} else {
		printf("    H   G   F   E   D   C   B   A\n");
	}
	printf("  +---+---+---+---+---+---+---+---+\n");
	for (i = 0; i < 8; i++) {
		if (i > 0) {
			printf("  +---+---+---+---+---+---+---+---+\n");
		}
		if (player == WHITE) {
			printf("%c", '8' - i);
		} else {
			printf("%c", '1' + i);
		}
		for (j = 0; j < 8; j++) {
			printf(j == 0 ? " |" : "|");
			if (player == WHITE) {
				row = i; col = j;
			} else {
				row = 7 - i; col = 7 - j;
			}
			Piece_print(b->fields[col][row]);
		}
		if (player == BLACK) {
			printf("| %c\n", '1' + i);
		} else {
			printf("| %c\n", '8' - i);
		}
	}
	printf("  +---+---+---+---+---+---+---+---+\n");
	if (player == WHITE) {
		printf("    A   B   C   D   E   F   G   H\n");
	} else {
		printf("    H   G   F   E   D   C   B   A\n");
	}
}
#endif

Piece *Board_get_piece(Board *b, int x, int y) {
	return b->fields[x][y];
}

Piece *Board_get_piece_safe(Board *b, int x, int y) {
	if (x < 0 || x > 7 || y < 0 || y > 7) {
		return NULL;
	}
	return b->fields[x][y];
}

bool Board_is_empty(Board *b, int x, int y) {
	return b->fields[x][y] == NULL;
}

bool Board_is_at(Board *b, int x, int y, int shape, int color) {
	Piece *p = Board_get_piece(b, x, y);
	return p != NULL && Piece_matches(p, shape, color);
}

bool Board_is_at_safe(Board *b, int x, int y, int shape, int color) {
	Piece *p = Board_get_piece_safe(b, x, y);
	return p != NULL && Piece_matches(p, shape, color);
}

bool Board_is_color(Board *b, int x, int y, int color) {
	Piece *p = Board_get_piece(b, x, y);
	return p != NULL && p->color == color;
}

bool Board_is_type(Board *b, int x, int y, int shape) {
	Piece *p = Board_get_piece(b, x, y);
	return p != NULL && p->shape == shape;
}

void Board_set(Board *b, int x, int y, Piece *p) {
	b->fields[x][y] = p;
}

void Board_remove_piece(Board *b, int x, int y) {
	if (b->fields[x][y] == NULL)
		return;
	Piece_destroy(b->fields[x][y]);
	b->fields[x][y] = NULL;
}

int Board_evaluate(Board *b) {
	return Fitness_calculate(b);
}

int Board_turn(Board *b) {
	return b->ply_count % 2 == 0 ? WHITE : BLACK;
}

bool Board_equals(bool quick, Board *left, Board *right) {
	int ok = quick ||
			((left->white_can_castle_kings_side == right->white_can_castle_kings_side)
			 && (left->white_can_castle_queens_side == right->white_can_castle_queens_side)
			 && (left->black_can_castle_kings_side == right->black_can_castle_kings_side)
			 && (left->black_can_castle_queens_side == right->black_can_castle_queens_side)
			 && (left->white_can_en_passant == right->white_can_en_passant)
			 && (left->black_can_en_passant == right->black_can_en_passant));
	if (!ok) {
		return false;
	}
	int i, j;
	for (i = 0; i < 8; i++) {
		for (j = 0;  j < 8; j++) {
			if (left->fields[i][j] == NULL && Board_get_piece(right,i,j) == NULL)
				continue;
			if (left->fields[i][j] == NULL || !Piece_equals(left->fields[i][j], Board_get_piece(right, i, j)))
				return false;
		}
	}
	return true;
}


UndoableMove *Board_do_move(Board *board, Move *move) {
	assert(board != NULL && move != NULL);

	int x = move->x, y = move->y;
	int xx = move->xx, yy = move->yy;
	Piece *piece, *target;
	UndoableMove *umove;


	piece = Board_get_piece(board, x, y);
	target = Board_get_piece(board, xx, yy);
	if (piece == NULL) {
		Board_print(board, WHITE);
		printf("Square: %d,%d -> %d,%d\n", x,y,xx,yy);
	}
	assert(piece != NULL);
	
	umove = Undo_create(
				x, y, xx, yy
				, yy
				, target
				, board->white_can_castle_queens_side 
				, board->white_can_castle_kings_side 
				, board->black_can_castle_queens_side 
				, board->black_can_castle_kings_side 
				, board->white_can_en_passant 
				, board->black_can_en_passant);
	board->black_can_en_passant = -1;
	board->white_can_en_passant = -1;

	if (target == NULL && piece->shape != PAWN) {
		board->fifty_move_count++;
		umove->adds_to_fifty = true;
	}
	// Castling:
	if (piece->shape == KING) {
		// When the King has moved, castling is no more possible
		if (piece->color == WHITE) {
			board->white_can_castle_kings_side = false;
			board->white_can_castle_queens_side = false;
		} else {
			board->black_can_castle_kings_side = false;
			board->black_can_castle_queens_side = false;
		}
		if (x == 4) {
			if (xx == 6) {
				// Move rook
				board->fields[5][y] = board->fields[7][y];
				board->fields[7][y] = NULL;
				umove->is_castling = true;
			} else if (xx == 2) {
				// Move rook
				board->fields[3][y] = board->fields[0][y];
				board->fields[0][y] = NULL;
				umove->is_castling = true;
			}
		}
	// Disable castling if rooks move:
	} else if (piece->shape == ROOK) {
		if (piece->color == WHITE) {
			if (board->fields[0][7] == NULL) {
				board->black_can_castle_queens_side = false;
			}
			if (board->fields[7][7] == NULL) {
				board->black_can_castle_kings_side = false;
			}
		} else {
			if (board->fields[0][0] == NULL) {
				board->black_can_castle_queens_side = false;
			}
			if (board->fields[7][0] == NULL) {
				board->black_can_castle_kings_side = false;
			}
		}
	// En-passant / pawn promotion
	} else if (piece->shape == PAWN) {
		// Check if this move enables the opponent to do en passant
		if ((y == 1 && yy == 3) || (y == 6 && yy == 4)) {
			// if pawn moves 2 steps forward, and directly to the left or
			// right of it there is an opposing pawn, that enemy pawn may perform
			// 'en passant' on the moving pawn.
			if ((x > 0 && board->fields[x-1][yy] != NULL && board->fields[x-1][yy]->color == -piece->color)
					|| (x < 7 && board->fields[x+1][yy] != NULL && board->fields[x+1][yy]->color == -piece->color)) {
				if (piece->color == WHITE) {
					board->black_can_en_passant = x;
				} else {
					board->white_can_en_passant = x;
				}
			}
		}
		// Check if this move is an en-passant move
		if (abs(x - xx) == 1 && abs(y - yy) == 2 && board->fields[xx][yy] == NULL && board->fields[xx][y]->shape == PAWN) {
			// if pawn moves diagonally while target tile is empty,
			// this move was an 'en passant' move. Remove the victim's body.
			umove->hit_y = y;
			umove->hit_piece = board->fields[xx][y];
			board->fields[xx][y] = NULL;
		}
		// Check if this move queenifies a pawn
		if ((piece->color == BLACK && yy == 7) || (piece->color == WHITE && yy == 0)) {
			int color = piece->color;
			Piece_destroy(piece);
			piece = Piece_create(move->promotion, color);
			umove->is_promotion = true;
		}
	}

	// Move the piece:
	board->fields[xx][yy] = piece;
	board->fields[x][y] = NULL;
	board->ply_count++;
	return umove;
}


void Board_undo_move(Board *board, UndoableMove *umove) {
	assert(umove != NULL);

	// Reposition the moved piece
	if (umove->is_promotion) {
		board->fields[umove->x][umove->y] =
			Piece_create(PAWN, board->fields[umove->xx][umove->yy]->color);
	} else {
		board->fields[umove->x][umove->y] = board->fields[umove->xx][umove->yy];
	}

	if (umove->adds_to_fifty) {
		board->fifty_move_count--;
	}

	// Restore any hit piece
	board->fields[umove->xx][umove->hit_y] = umove->hit_piece;
	
	// Check for castling
	if (umove->is_castling) {
		if (umove->xx == 2) {
			board->fields[0][umove->y] = board->fields[3][umove->y];
			board->fields[3][umove->y] = NULL;
		} else {
			board->fields[7][umove->y] = board->fields[5][umove->y];
			board->fields[5][umove->y] = NULL;
		}
	}
	
	board->white_can_castle_queens_side = umove->white_can_castle_queens_side;
	board->white_can_castle_kings_side = umove->white_can_castle_kings_side;
	board->black_can_castle_queens_side = umove->black_can_castle_queens_side;
	board->black_can_castle_kings_side = umove->black_can_castle_kings_side;
	board->white_can_en_passant = umove->white_can_en_passant;
	board->black_can_en_passant = umove->black_can_en_passant;
	board->ply_count--;
}


void Board_save(Board *board, const char *filename) {
	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		fprintf(stderr, "Error opening file!");
		exit(1);
	}
	int i,j;
	Piece *p;
	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			p = Board_get_piece(board, i, j);
			if (p == NULL) {
				fprintf(file, "--");
			} else {
				fprintf(file, "%s", p->symbol);
			}
		}
		fprintf(file, "\n");
	}
	fprintf(file, "\n%d %d %d %d %d %d %d %d %d %d %d\n",
		board->white_can_castle_queens_side,
		board->white_can_castle_kings_side,
		board->black_can_castle_queens_side,
		board->black_can_castle_kings_side,
		board->white_can_en_passant,
		board->black_can_en_passant,
		board->ply_count,
		board->fifty_move_count,
		board->state,
		board->captures_white_count,
		board->captures_black_count);

	Capture *curr = board->captures_white;
	while(curr) {
		fprintf(file, "%s", curr->piece->symbol);
		curr = curr->next_sibling;
	}
	Capture *curr2 = board->captures_black;
	while(curr2) {
		fprintf(file, "%s", curr2->piece->symbol);
		curr2 = curr2->next_sibling;
	}
	fclose(file);
}

static int read_two_chars(FILE *file, char *buf) {
	buf[0] = (char) fgetc(file);
	if (buf[0] == EOF) {
		return false;
	}
	buf[1] = (char) fgetc(file);
	return buf[1] != EOF;
}

Board *Board_read(const char *filename) {
	FILE *file;
	char *buf = malloc(2 * sizeof(char));
	Board *board = malloc(sizeof(Board));
	Board_reset(board);

	file = fopen(filename, "r");
	if (file) {
		int i, j;
		for (j = 0; j < 8; j++) {
			for (i = 0; i < 8; i++) {
				int ok = read_two_chars(file, buf);
				if (!ok) {
					fprintf(stderr, "Unexpected EOF!");
					exit(1);
				}
				board->fields[i][j] = Piece_parse(buf);
			}
			// Consume newline
			fgetc(file);
		}
		int cap_white, cap_black, wccqs, wccks, bccqs, bccks, wcep, bcep, ply, fif, state;
		int ok = fscanf(file, "\n%d %d %d %d %d %d %d %d %d %d %d\n",
			&wccqs,
			&wccks,
			&bccqs,
			&bccks,
			&wcep,
			&bcep,
			&ply,
			&fif,
			&state,
			&cap_white,
			&cap_black);
    	if (ferror(file) || !ok) {
        	fprintf(stderr, "Error reading file!");
        	exit(1);
		}

		board->white_can_castle_queens_side = wccqs;
		board->white_can_castle_kings_side = wccks;
		board->black_can_castle_queens_side = bccqs;
		board->black_can_castle_kings_side = bccks;
		board->white_can_en_passant = wcep;
		board->black_can_en_passant = bcep;
		board->ply_count = ply;
		board->fifty_move_count = fif;
		board->state = state;

		// Captured pieces
		for(i = 0; i < cap_white; i++) {
			ok = read_two_chars(file, buf);
			if (!ok) {
				fprintf(stderr, "Unexpected EOF while reading %d captured white pieces!", board->captures_white_count);
				exit(1);
			}
			Board_add_captured_piece(board, Piece_parse(buf));
		}
		for(i = 0; i < cap_black; i++) {
			ok = read_two_chars(file, buf);
			if (!ok) {
				fprintf(stderr, "Unexpected EOF while reading %d captured black pieces!", board->captures_black_count);
				exit(1);
			}
			Board_add_captured_piece(board, Piece_parse(buf));
		}
		fclose(file);
	} else {
		fprintf(stderr, "Unable to open file for reading!");
		exit(1);
	}

	return board;
}

void Board_add_capture(Board *board, UndoableMove *um) {
	if (um->hit_piece == NULL) {
		return;
	}
	Board_add_captured_piece(board, um->hit_piece);
}

void Board_add_captured_piece(Board *board, Piece *piece) {
	Capture *c = malloc(sizeof(Capture));
	c->piece = piece;
	c->next_sibling = NULL;
	if (piece->color == WHITE) {
		if (board->captures_white != NULL) {
			c->next_sibling = board->captures_white;
		}
		board->captures_white_count++;
		board->captures_white = c;
	} else {
		if (board->captures_black != NULL) {
			c->next_sibling = board->captures_black;
		}
		board->captures_black_count++;
		board->captures_black = c;
	}
}

void Capture_destroy(Capture *capture) {
	assert(capture != NULL);
	Piece_destroy(capture->piece);
	if (capture->next_sibling != NULL) {
		Capture_destroy(capture->next_sibling);
	}
	free(capture);
}
