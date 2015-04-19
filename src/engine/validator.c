#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "datatypes.h"
#include "algebraicnotation.h"
#include "move.h"
#include "piece.h"
#include "square.h"
#include "board.h"
#include "validator.h"

#ifndef _VALIDATOR_C_
#define _VALIDATOR_C_

static bool contains(Move *first, Move *needle);

static void add_move(Move **head, Move *move);

static void add_move_pawn(Move **head, int color, int x, int y, int xx, int yy);

static int get_all_valid_moves_of_piece(Move **head, Board *board, int i, int j, bool only_count);

static int get_valid_moves_pawn(Move **head, Board *board, int x, int y, int color, bool only_count);

static int get_valid_moves_knight(Move **head, Board *board, int x, int y, int color, bool only_count);

static int get_valid_moves_rook(Move **head, Board *board, int x, int y, int color, bool only_count);

static int get_valid_moves_bishop(Move **head, Board *board, int x, int y, int color, bool only_count);

static int get_valid_moves_queen(Move **head, Board *board, int x, int y, int color, bool only_count);

static int get_valid_moves_king(Move **head, Board *board, int x, int y, int color, bool only_count);

static bool gives_check(Board *board, Move *move, int color);

static void add_square(Square **head, Square *square);

#endif

static bool contains(Move *first, Move *needle) {
	Move *curr;
	curr = first;
	while(curr) {
		if (Move_equals(curr, needle)) {
			return true;
		}
		curr = curr->next_sibling;
	}
	return false;
}

static bool gives_check(Board *board, Move *move, int color) {
	if (Move_is_nullmove(move)) {
		return false;
	}
	UndoableMove *umove = Board_do_move(board, move);
	int kingI = -1, kingJ = -1;
	int i, j;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (Board_is_at(board, i, j, KING, color)) {
				kingI = i; kingJ = j;
				break;
			}
		}
	}
	if (kingI == -1) {
		Board_undo_move(board, umove);
		Undo_destroy(umove);
		return true;
	}
	int result = v_square_gives_check(board, kingI, kingJ, color);
	Board_undo_move(board, umove);
	Undo_destroy(umove);
	return result;
}


bool v_king_at_check(Board *board, int color) {
	int kingI = -1, kingJ = -1;
	int i, j;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (Board_is_at(board, i, j, KING, color)) {
				kingI = i; kingJ = j;
			}
		}
	}
	if (kingI == -1) {
		return true;
	}
	return v_square_gives_check(board, kingI, kingJ, color);
}


bool v_square_gives_check(Board *board, int x, int y, int color) {
	// Pawns
	if (color== WHITE) {
		if (Board_is_at_safe(board, x-1, y-1, PAWN, BLACK) || Board_is_at_safe(board, x+1, y-1, PAWN, BLACK)) {
			return true;
		}
	} else if (color== BLACK) {
		if (Board_is_at_safe(board, x-1, y+1, PAWN, WHITE) || Board_is_at_safe(board, x+1, y+1, PAWN, WHITE)) {
			return true;
		}
	}
	// Knights
	if (Board_is_at_safe(board, x-1, y-2, KNIGHT, -color)
		|| Board_is_at_safe(board, x+1, y-2, KNIGHT, -color)
		|| Board_is_at_safe(board, x-1, y+2, KNIGHT, -color)
		|| Board_is_at_safe(board, x+1, y+2, KNIGHT, -color)
		|| Board_is_at_safe(board, x-2, y-1, KNIGHT, -color)
		|| Board_is_at_safe(board, x+2, y-1, KNIGHT, -color)
		|| Board_is_at_safe(board, x-2, y+1, KNIGHT, -color)
		|| Board_is_at_safe(board, x+2, y+1, KNIGHT, -color)) {
		return true;
	}

	int i, j;
	Piece *p;
	// Rook and Queen
	i = x - 1; // left
	while (i >= 0) {
		p = Board_get_piece(board, i, y);
		if (p != NULL) {
			if (p->color == -color && (p->shape == ROOK || p->shape == QUEEN)) {
				return true;
			} else {
				break;
			}
		}
		i = i - 1;
	}
	i = x + 1; // right
	while (i <= 7) {
		p = Board_get_piece(board, i, y);
		if (p != NULL) {
			if (p->color == -color && (p->shape == ROOK || p->shape == QUEEN)) {
				return true;
			} else {
				break;
			}
		}
		i = i + 1;
	}
	i = y - 1;
	while (i >= 0) {
		p = Board_get_piece(board, x, i);
		if (p != NULL) {
			if (p->color == -color && (p->shape == ROOK || p->shape == QUEEN)) {
				return true;
			} else {
				break;
			}
		}
		i = i - 1;
	}
	i = y + 1;
	while (i <= 7) {
		p = Board_get_piece(board, x, i);
		if (p != NULL) {
			if (p->color == -color && (p->shape == ROOK || p->shape == QUEEN)) {
				return true;
			} else {
				break;
			}
		}
		i = i + 1;
	}
	// Bishop && Queen
	i = x - 1; // top left
	j = y - 1;
	while (i >= 0 && j >= 0) {
		p = Board_get_piece(board, i, j);
		if (p != NULL) {
			if (p->color == -color && (p->shape == BISHOP || p->shape == QUEEN)) {
				return true;
			} else {
				break;
			}
		}
		i = i - 1;
		j = j - 1;
	}
	i = x - 1; // bottom left
	j = y + 1;
	while (i >= 0 && j <= 7) {
		p = Board_get_piece(board, i, j);
		if (p != NULL) {
			if (p->color == -color && (p->shape == BISHOP || p->shape == QUEEN)) {
				return true;
			} else {
				break;
			}
		}
		i = i - 1;
		j = j + 1;
	}
	i = x + 1; // top right
	j = y - 1;
	while (i <= 7 && j >= 0) {
		p = Board_get_piece(board, i, j);
		if (p != NULL) {
			if (p->color == -color && (p->shape == BISHOP || p->shape == QUEEN)) {
				return true;
			} else {
				break;
			}
		}
		i = i + 1;
		j = j - 1;
	}
	i = x + 1;
	j = y + 1;
	while (i <= 7 && j <= 7) {
		p = Board_get_piece(board, i, j);
		if (p != NULL) {
			if (p->color == -color && (p->shape == BISHOP || p->shape == QUEEN)) {
				return true;
			} else {
				break;
			}
		}
		i = i + 1;
		j = j + 1;
	}
	// King
	if (Board_is_at_safe(board, x - 1, y, KING, -color)
		|| Board_is_at_safe(board, x - 1, y - 1, KING, -color)
		|| Board_is_at_safe(board, x - 1, y + 1, KING, -color)
		|| Board_is_at_safe(board, x + 1, y, KING, -color)
		|| Board_is_at_safe(board, x + 1, y - 1, KING, -color)
		|| Board_is_at_safe(board, x + 1, y + 1, KING, -color)
		|| Board_is_at_safe(board, x, y - 1, KING, -color)
		|| Board_is_at_safe(board, x, y + 1, KING, -color)) {
		return true;
	}
	// Done.
	return false;
}


int v_get_rough_move_count_for_piece(Board *board, int x, int y) {
	return get_all_valid_moves_of_piece(NULL, board, x, y, true);
}


bool v_is_valid_move(Board *board, Move *move) {
	// Nothing moving?
	if (move->x == move->xx && move->y == move->yy)
		return false;
	// Right player?
	if (Board_get_piece(board, move->x, move->y)->color != Board_turn(board)) {
		return false;
	}
	// A move is valid if it can be made and does not result in check of the current player
	Move *validmoves = Move_alloc();
	get_all_valid_moves_of_piece(&validmoves, board, move->x, move->y, false);
	int is_valid = contains(validmoves, move);
	int result = is_valid && !gives_check(board, move, Board_get_piece(board, move->x, move->y)->color);
	Move_destroy(validmoves);
	return result;
}


int v_get_all_valid_moves_for_color(Move **head, Board *board, int color) {
	int i, j;
	int count = 0;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (Board_is_color(board, i, j, color)) {
				count += get_all_valid_moves_of_piece(head, board, i, j, false);
			}
		}
	}
	// Only happens when add_move is not called, i.e. no valid moves are found.
	if (Move_is_nullmove(*head) && count > 0) {
		fprintf(stderr, "Warning: v_get_all_valid_moves_for_color (length %d) starts with nullmove\n", count);
		return 0;
	}
	return count;
}


static int get_all_valid_moves_of_piece(Move **head, Board *board, int i, int j, bool only_count) {
	Piece *piece = Board_get_piece(board, i, j);
	int count = 0;
	// Get all possible moves for this piece
	if (piece->shape == PAWN) {
		count = get_valid_moves_pawn(head, board, i, j, piece->color, only_count);
	} else if (piece->shape == KNIGHT) {
		count = get_valid_moves_knight(head, board, i, j, piece->color, only_count);
	} else if (piece->shape == BISHOP) {
		count = get_valid_moves_bishop(head, board, i, j, piece->color, only_count);
	} else if (piece->shape == ROOK) {
		count = get_valid_moves_rook(head, board, i, j, piece->color, only_count);
	} else if (piece->shape == QUEEN) {
		count = get_valid_moves_queen(head, board, i, j, piece->color, only_count);
	} else if (piece->shape == KING) {
		count = get_valid_moves_king(head, board, i, j, piece->color, only_count);
	}

	// When only_count enabled, skip the expensive checks and
	// immediately return the rough estimate.
	if (head == NULL) {
		assert(only_count);
		return count;
	}

	// Remove the moves that put the king in check
	Move *curr = *head;
	Move *prev = NULL;
	int iter = 0;
	int total = count;
	while (curr && iter < total) {
		if (gives_check(board, curr, piece->color)) {
			// Remove move by making the previous move
			// point to the next of the current
			if (prev == NULL) {
				*head = curr->next_sibling;
			} else {
				prev->next_sibling = curr->next_sibling;
			}
			count--;
		} else {
			// Remember which ones put opponent in check
			if (!only_count && gives_check(board, curr, -piece->color)) {
				curr->gives_check = true;
			}
			prev = curr;
		}
		curr = curr->next_sibling;
		iter++;
	}
	return count;
}


static void add_move(Move **head, Move *move) {
	// Case 1: not adding moves, just counting.
	if (head == NULL) {
		return;
	// Case 2: List is empty. Insert move as first item.
	} else if (Move_is_nullmove(*head)) {
		Move_destroy(*head);
		*head = move;
	// Case 3: insert move *before* head
	} else {
		move->next_sibling = *head;
		*head = move;
	}
}


static void add_move_pawn(Move **head, int color, int x, int y, int xx, int yy) {
	if (head == NULL) {
		return;
	} else if (yy == 7 || yy == 0) {
		add_move(head, Move_create(color, x, y, xx, yy, QUEEN));
		add_move(head, Move_create(color, x, y, xx, yy, KNIGHT));
	} else {
		add_move(head, Move_create(color, x, y, xx, yy, 0));
	}
}


static int get_valid_moves_pawn(Move **head, Board *board, int x, int y, int color, bool only_count) {
	int xx, yy;
	int startY = color == WHITE ? 6 : 1;
	int count = 0;
	if (Board_get_piece(board, x, y - color) == NULL) {
		if(!only_count) add_move_pawn(head, color, x, y, x, y - color);
		count++;
		if (y == startY && Board_get_piece(board, x, y - 2*color) == NULL) {
			if(!only_count) add_move(head, Move_create(color, x, y, x, y - 2*color, 0));
			count++;
		}
	}
	xx = x-1; yy = y - color;
	if (x > 0 && Board_is_color(board, xx, yy, -color)) {
		if(!only_count) add_move_pawn(head, color, x, y, xx, yy);
		count++;
	}
	xx = x+1; yy = y - color;
	if (x < 7 && Board_is_color(board, xx, yy, -color)) {
		if(!only_count) add_move_pawn(head, color, x, y, xx, yy);
		count++;
	}
	// En passant)
	if (color == WHITE) {
		if (board->white_can_en_passant >= 0 && y == RANK_5 &&
				(x == board->white_can_en_passant - 1 || x == board->white_can_en_passant + 1)) {
			if(!only_count) add_move(head, Move_create(color, x, y, board->white_can_en_passant, y - 1, 0));
			count++;
		}
	} else {
		if (board->black_can_en_passant >= 0 && y == RANK_4 &&
				(x == board->black_can_en_passant - 1 || x == board->black_can_en_passant + 1)) {
			if(!only_count) add_move(head, Move_create(color, x, y, board->black_can_en_passant, y+1, 0));
			count++;
		}
	}
	return count;
}


static int get_valid_moves_knight(Move **head, Board *board, int x, int y, int color, bool only_count) {
	int xx, yy;
	int count = 0;
	if (x > 0) {
		xx = x-1; yy = y-2;
		if (y > 1 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
			if(!only_count) add_move(head, Move_create(color, x, y, xx, yy, 0));
			count++;
		}
		xx = x-1; yy = y+2;
		if (y < 6 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
			if(!only_count) add_move(head, Move_create(color, x, y, xx, yy, 0));
			count++;
		}
		if (x > 1) {
			xx = x-2; yy = y-1;
			if (y > 0 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
				if(!only_count) add_move(head, Move_create(color, x, y, xx, yy, 0));
				count++;
			}
			xx = x-2; yy = y+1;
			if (y < 7 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
				if(!only_count) add_move(head, Move_create(color, x, y, xx, yy, 0));
				count++;
			}
		}
	}
	if (x < 7) {
		xx = x+1; yy = y-2;
		if (y > 1 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
			if(!only_count) add_move(head, Move_create(color, x, y,xx, yy, 0));
			count++;
		}
		xx = x+1; yy = y+2;
		if (y < 6 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
			if(!only_count) add_move(head, Move_create(color, x, y,xx, yy, 0));
			count++;
		}
		if (x < 6) {
			xx = x+2; yy = y-1;
			if (y>0 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
				if(!only_count) add_move(head, Move_create(color, x, y,xx, yy, 0));
				count++;
			}
			xx = x+2; yy = y+1;
			if (y<7 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
				if(!only_count) add_move(head, Move_create(color, x, y,xx, yy, 0));
				count++;
			}
		}
	}
	return count;
}
		

static int get_valid_moves_rook(Move **head, Board *board, int x, int y, int color, bool only_count) {
	int i, j;
	int count = 0;
	i = x-1; // Left
	while (i >= 0 && (Board_get_piece(board, i, y) == NULL || Board_get_piece(board, i, y)->color != color)) {
		if(!only_count) add_move(head, Move_create(color, x, y, i, y, 0));
		count++;
		if (Board_get_piece(board, i, y) != NULL)
			break;
		i = i - 1;
	}
	i = x + 1; // right
	while (i <= 7 && (Board_get_piece(board, i, y) == NULL || Board_get_piece(board, i, y)->color != color)) {
		if(!only_count) add_move(head, Move_create(color, x, y, i, y, 0));
		count++;
		if (Board_get_piece(board, i, y) != NULL)
			break;
		i = i + 1;
	}
	j = y - 1; // top
	while (j >= 0 && (Board_get_piece(board, x, j) == NULL || Board_get_piece(board, x, j)->color != color)) {
		if(!only_count) add_move(head, Move_create(color, x, y, x, j, 0));
		count++;
		if (Board_get_piece(board, x, j) != NULL)
			break;
		j = j - 1;
	}		
	j = y + 1; // bottom
	while (j <= 7 && (Board_get_piece(board, x, j) == NULL || Board_get_piece(board, x, j)->color != color)) {
		if(!only_count) add_move(head, Move_create(color, x, y, x, j, 0));
		count++;
		if (Board_get_piece(board, x, j) != NULL)
			break;
		j = j + 1;
	}
	return count;
}

static int get_valid_moves_bishop(Move **head, Board *board, int x, int y, int color, bool only_count) {
	int i, j;
	int count = 0;
	i = x - 1;
	j = y - 1;
	while (i >= 0 && j >= 0 && (Board_get_piece(board, i, j) == NULL || Board_get_piece(board, i, j)->color != color)) {
		if(!only_count) add_move(head, Move_create(color, x, y, i, j, 0));
		count++;
		if (Board_get_piece(board, i, j) != NULL)
			break;
		i = i - 1;
		j = j - 1;
	}
	i = x - 1;
	j = y + 1;
	while (i >= 0 && j <= 7 && (Board_get_piece(board, i, j) == NULL || Board_get_piece(board, i, j)->color != color)) {
		if(!only_count) add_move(head, Move_create(color, x, y, i, j, 0));
		count++;
		if (Board_get_piece(board, i, j) != NULL)
			break;
		i = i - 1;
		j = j + 1;
	}
	i = x + 1;
	j = y - 1;
	while (i <= 7 && j >= 0 && (Board_get_piece(board, i, j) == NULL || Board_get_piece(board, i, j)->color != color)) {
		if(!only_count) add_move(head, Move_create(color, x, y, i, j, 0));
		count++;
		if (Board_get_piece(board, i, j) != NULL)
			break;
		i = i + 1;
		j = j - 1;
	}
	i = x + 1;
	j = y + 1;
	while (i <= 7 && j <= 7 && (Board_get_piece(board, i, j) == NULL || Board_get_piece(board, i, j)->color != color)) {
		if(!only_count) add_move(head, Move_create(color, x, y, i, j, 0));
		count++;
		if (Board_get_piece(board, i, j) != NULL)
			break;
		i = i + 1;
		j = j + 1;
	}
	return count;
}


static int get_valid_moves_queen(Move **head, Board *board, int x, int y, int color, bool only_count) {
	int count;
	count = get_valid_moves_rook(head, board, x, y, color, only_count);
	count += get_valid_moves_bishop(head, board, x, y, color, only_count);
	return count;
}


static int get_valid_moves_king(Move **head, Board *board, int x, int y, int color, bool only_count) {
	int xx, yy;
	int count = 0;
	if (x >= 1) {
		xx = x-1; yy = y;
		if (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color) {
			if(!only_count) add_move(head, Move_create(color, x, y,  xx, yy, 0));
			count++;
		}
		xx = x-1; yy = y-1;
		if (y >= 1 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
			if(!only_count) add_move(head, Move_create(color, x, y,  xx, yy, 0));
			count++;
		}
		xx = x-1; yy = y+1;
		if (y<=6 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
			if(!only_count) add_move(head, Move_create(color, x, y,  xx, yy, 0));
			count++;
		}
	}
	if (x <= 6) {
		xx = x+1; yy = y;
		if (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color) {
			if(!only_count) add_move(head, Move_create(color, x, y,  xx, yy, 0));
			count++;
		}
		xx = x+1; yy = y-1;
		if (y >= 1 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
			if(!only_count) add_move(head, Move_create(color, x, y,  xx, yy, 0));
			count++;
		}
		xx = x+1; yy = y+1;
		if (y<=6 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
			if(!only_count) add_move(head, Move_create(color, x, y,  xx, yy, 0));
			count++;
		}
	}
	xx = x; yy = y-1;
	if (y >= 1 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
		if(!only_count) add_move(head, Move_create(color, x, y,  xx, yy, 0));
		count++;
	}
	xx = x; yy = y+1;
	if (y <= 6 && (Board_get_piece(board, xx, yy) == NULL || Board_get_piece(board, xx, yy)->color != color)) {
		if(!only_count) add_move(head, Move_create(color, x, y,  xx, yy, 0));
		count++;
	}
	// Castling)
	if (color == BLACK && y == 0 && x == 4 && !v_square_gives_check(board, 4, 0, color)) {
		if (board->black_can_castle_queens_side
				&& Board_is_at(board, 0, 0, ROOK, color)
				&& Board_get_piece(board, 1, 0) == NULL
				&& Board_get_piece(board, 2, 0) == NULL
				&& Board_get_piece(board, 3, 0) == NULL
				&& !v_square_gives_check(board, 1, 0, BLACK)
				&& !v_square_gives_check(board, 2, 0, BLACK)
				&& !v_square_gives_check(board, 3, 0, BLACK)) {
				if(!only_count) add_move(head, Move_create(color, x, y, x-2, y, 0));
			count++;
		}
		if (board->black_can_castle_kings_side
				&& Board_is_at(board, 7, 0, ROOK, color)
				&& Board_get_piece(board, 5, 0) == NULL
				&& Board_get_piece(board, 6, 0) == NULL
				&& !v_square_gives_check(board, 5, 0, BLACK)
				&& !v_square_gives_check(board, 6, 0, BLACK)) {
			if(!only_count) add_move(head, Move_create(color, x, y, x+2, y, 0));
			count++;
		}
	} else if (color == WHITE && y == 7 && x == 4 && !v_square_gives_check(board, 4, 7, color)) {
		if (board->white_can_castle_queens_side
				&& Board_is_at(board, 0, 7, ROOK, color)
				&& Board_get_piece(board, 1, 7) == NULL
				&& Board_get_piece(board, 2, 7) == NULL
				&& Board_get_piece(board, 3, 7) == NULL
				&& !v_square_gives_check(board, 1, 7, WHITE)
				&& !v_square_gives_check(board, 2, 7, WHITE)
				&& !v_square_gives_check(board, 3, 7, WHITE)) {
			if(!only_count) add_move(head, Move_create(color, x, y, x-2, y, 0));
			count++;
		}
		if (board->white_can_castle_kings_side
				&& Board_is_at(board, 7, 7, ROOK, color)
				&& Board_get_piece(board, 5, 7) == NULL
				&& Board_get_piece(board, 6, 7) == NULL
				&& !v_square_gives_check(board, 5, 7, WHITE)
				&& !v_square_gives_check(board, 6, 7, WHITE)) {
			if(!only_count) add_move(head, Move_create(color, x, y, x+2, y, 0));
			count++;
		}
	}
	return count;
}


static void add_square(Square **head, Square *square) {
	// List is empty
	if (*head == NULL || (*head)->piece == NULL) {
		Square_destroy(*head);
		*head = square;
	// Else, insert right after head
	} else {
		// If head is not the last, re-attach the tail
		if ((*head)->next_sibling != NULL) {
			square->next_sibling = (*head)->next_sibling;
		}
		// Insert
		(*head)->next_sibling = square;
	}
}


int v_square_attacked_by(Square **head, Board *board, int x, int y, int color, int shape) {
	int count = 0;
	if(shape == PAWN && !Board_is_empty(board, x, y)) {
		// Pawns
		if(Board_is_at_safe(board, x - 1, y - color, PAWN, -color)) {
			add_square(head, Square_create(x - 1, y - color, board));
			count++;
		}
		if(Board_is_at_safe(board, x + 1, y - color, PAWN, -color)) {
			add_square(head, Square_create(x + 1, y - color, board));
			count++;
		}
	} else if(shape == KNIGHT) {
		// Knights
		if(Board_is_at_safe(board, x - 1, y - 2, KNIGHT, -color)) {
			add_square(head, Square_create(x - 1, y- 2, board));
			count++;
		}
		if(Board_is_at_safe(board, x + 1, y - 2, KNIGHT, -color)) {
			add_square(head, Square_create(x + 1, y- 2, board));
			count++;
		}
		if(Board_is_at_safe(board, x - 1, y + 2, KNIGHT, -color)) {
			add_square(head, Square_create(x - 1, y+ 2, board));
			count++;
		}
		if(Board_is_at_safe(board, x + 1, y + 2, KNIGHT, -color)) {
			add_square(head, Square_create(x + 1, y + 2, board));
			count++;
		}
		if(Board_is_at_safe(board, x - 2, y - 1, KNIGHT, -color)) {
			add_square(head, Square_create(x - 2, y - 1, board));
			count++;
		}
		if(Board_is_at_safe(board, x + 2, y - 1, KNIGHT, -color)) {
			add_square(head, Square_create(x + 2, y - 1, board));
			count++;
		}
		if(Board_is_at_safe(board, x - 2, y + 1, KNIGHT, -color)) {
			add_square(head, Square_create(x - 2, y + 1, board));
			count++;
		}
		if(Board_is_at_safe(board, x + 2, y + 1, KNIGHT, -color)) {
			add_square(head, Square_create(x + 2, y + 1, board));
			count++;
		}
	} else if(shape == ROOK || shape == QUEEN) {
		// Rook and Queen
		int i = x-1;
		while(i >= 0) {
			if(Board_is_at(board, i, y, shape, -color)) {
				add_square(head, Square_create(i, y, board));
				count++;
				break;
			}
			if(!Board_is_empty(board, i, y)) {
				break;
			}
			i--;
		}
		i = x + 1;
		while(i <= 7) {
			if(Board_is_at(board, i, y, shape, -color)) {
				add_square(head, Square_create(i, y, board));
				count++;
				break;
			}
			if(!Board_is_empty(board, i, y)) {
				break;
			}
			i++;
		}
		i = y - 1;
		while(i >= 0) {
			if(Board_is_at(board, x, i, shape, -color)) {
				add_square(head, Square_create(x, i, board));
				count++;
				break;
			}
			if(!Board_is_empty(board, x, i)) {
				break;
			}
			i--;
		}
		i = y + 1;
		while(i <= 7) {
			if(Board_is_at(board, x, i, shape, -color)) {
				add_square(head, Square_create(x, i, board));
				count++;
				break;
			}
			if(!Board_is_empty(board, x, i)) {
				break;
			}
			i++;
		}
	} else if(shape == BISHOP || shape == QUEEN) {
		// Bishop and Queen
		int i, j;
		i = x - 1; j = y - 1;
		while(i >= 0 && j >= 0) {
			if(Board_is_at(board, i, j, shape, -color)) {
				add_square(head, Square_create(i, j, board));
				count++;
				break;
			}
			if(!Board_is_empty(board, i, j)) {
				break;
			}
			i--; j--;
		}
		i = x - 1; j = y + 1;
		while(i >= 0 && j <= 7) {
			if(Board_is_at(board, i, j, shape, -color)) {
				add_square(head, Square_create(i, j, board));
				count++;
				break;
			}
			if(!Board_is_empty(board, i, j)) {
				break;
			}
			i--; j++;
		}
		i = x + 1; j = y - 1;
		while(i <= 7 && j >= 0) {
			if(Board_is_at(board, i, j, shape, -color)) {
				add_square(head, Square_create(i, j, board));
				count++;
				break;
			}
			if(!Board_is_empty(board, i, j)) {
				break;
			}
			i++; j--;
		}
		i = x + 1; j = y + 1;
		while(i <= 7 && j <= 7) {
			if(Board_is_at(board, i, j, shape, -color)) {
				add_square(head, Square_create(i, j, board));
				count++;
				break;
			}
			if(!Board_is_empty(board, i, j)) {
				break;
			}
			i++; j++;
		}
	} else if(shape == KING) {
		// King
		if(x > 0) {
			if(Board_is_at(board, x - 1, y, KING, -color)) {
				add_square(head, Square_create(x - 1, y, board));
				count++;
			}
			if(y>0 && Board_is_at(board, x - 1, y - 1, KING, -color)) {
				add_square(head, Square_create(x - 1, y - 1, board));
				count++;
			}
			if(y<7 && Board_is_at(board, x - 1, y + 1, KING, -color)) {
				add_square(head, Square_create(x - 1, y + 1, board));
				count++;
			}
		}
		if(x < 7) {
			if(Board_is_at(board, x + 1, y, KING, -color)) {
				add_square(head, Square_create(x + 1, y, board));
				count++;
			}
			if(y>0 && Board_is_at(board, x + 1, y - 1, KING, -color)) {
				add_square(head, Square_create(x + 1, y- 1, board));
				count++;
			}
			if(y<7 && Board_is_at(board, x + 1, y + 1, KING, -color)) {
				add_square(head, Square_create(x + 1, y + 1, board));
				count++;
			}
		}
		if(y > 0 && Board_is_at(board, x , y - 1 , KING, -color)) {
			add_square(head, Square_create(x, y - 1, board));
			count++;
		}
		if(y < 7 && Board_is_at(board, x , y + 1 , KING, -color)) {
			add_square(head, Square_create(x, y + 1, board));
			count++;
		}
	}
	return count;
}