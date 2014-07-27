#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "datatypes.h"
#include "board.h"
#include "square.h"


Square *Square_create(int x, int y, Board *board) {
	Square *s = malloc(sizeof(Square));
	s->x = x;
	s->y = y;
	s->piece = Board_get_piece(board, x, y);
	s->next_sibling = NULL;
	return s;
}


void Square_destroy(Square *s) {
	if (s == NULL) {
		return;
	}
	if (s->next_sibling != NULL) {
		Square_destroy(s->next_sibling);
	}
	free(s);
}


int Square_is_at(Square *s, int x, int y) {
	return s->x == x && s->y == y;
}


char *Square_coords_to_string(int x, int y) {
	char *str = malloc(3 * sizeof(char));
	str[0] = x + 'a';
	str[1] = '8' - y;
	str[2] = '\0';
	return str;
}

char *Square_to_string(Square *s) {
	return Square_coords_to_string(s->x, s->y);
}


void Square_print(Square *s) {
	printf("%s\n", Square_to_string(s));
}


Square *Square_parse(char *tile) {
	if (strlen(tile) != 2) {
		return NULL;
	}
	Square *s = malloc(sizeof(Square));
	int file = tile[0] - 'a';
	int rank = 8 - (tile[1] - '0');
	if (file < 0 || file > 7 || rank < 0 || rank > 7) {
		return NULL;
	}
	s->x = file;
	s->y = rank;
	s->piece = NULL;
	s->next_sibling = NULL;
	return s;
}