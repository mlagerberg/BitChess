#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "color.h"
#include "datatypes.h"
#include "piece.h"


Piece *Piece_create(int shape, int color) {
	Piece *p = malloc(sizeof(Piece));
	p->color = color;
	p->shape = shape;
	if (color == WHITE) {
		switch(shape) {
			case PAWN:		p->symbol = strdup("wp");	break;
			case ROOK:		p->symbol = strdup("wR");	break;
			case KNIGHT:	p->symbol = strdup("wN");	break;
			case BISHOP:	p->symbol = strdup("wB");	break;
			case QUEEN:		p->symbol = strdup("wQ");	break;
			case KING: 		p->symbol = strdup("wK");	break;
		}
	} else {
		switch(shape) {
			case PAWN:		p->symbol = strdup("bp");	break;
			case ROOK:		p->symbol = strdup("bR");	break;
			case KNIGHT:	p->symbol = strdup("bN");	break;
			case BISHOP:	p->symbol = strdup("bB");	break;
			case QUEEN:		p->symbol = strdup("bQ");	break;
			case KING: 		p->symbol = strdup("bK");	break;
		}
	}
	return p;
}

Piece *Piece_clone(Piece *src) {
	Piece *p = malloc(sizeof(Piece));
	p->color = src->color;
	p->shape = src->shape;
	p->symbol = strdup(src->symbol);
	return p;
}

Piece *Piece_parse(char *symbol) {
	//Piece *p = malloc(sizeof(Piece));
	//p->symbol = strdup(symbol);
	int color;
	if (symbol[0] == 'b') {
		color = BLACK;
	} else if (symbol[0] == 'w') {
		color = WHITE;
	} else if (symbol[0] == '-') {
		return NULL;
	} else {
		fprintf(stderr, "Invalid player color while parsing piece: %c", *symbol);
		exit(1);
	}
	switch(symbol[1]) {
		case 'p':	return Piece_create(PAWN, color);
		case 'R':	return Piece_create(ROOK, color);
		case 'N':	return Piece_create(KNIGHT, color);
		case 'B':	return Piece_create(BISHOP, color);
		case 'Q':	return Piece_create(QUEEN, color);
		case 'K': 	return Piece_create(KING, color);
		default:
			fprintf(stderr, "Invalid piece shape while parsing piece: %c", symbol[1]);
			exit(1);
	}
	return NULL;
}

void Piece_destroy(Piece *p) {
	assert(p != NULL);
	free(p->symbol);
	free(p);
}

char *Piece_short_name(Piece *p) {
	return p->symbol;
}

bool Piece_matches(Piece *p, int shape, int color) {
	return p->shape == shape && p->color == color;
}

bool Piece_equals(Piece *left, Piece *right) {
	if (right == NULL) return false;
	return Piece_matches(left, right->shape, right->color);
}

void Piece_print(Piece *p) {
	if (p == NULL) {
		#ifdef UNICODE_OUTPUT
		printf(" ");
		#else
		printf("   ");
		#endif
	} else {
		#ifdef UNICODE_OUTPUT
		if (p->color == WHITE) {
			printf("%s", color_white);
		} else {
			printf("%s", color_black);
		}
		switch(p->shape) {
			case PAWN:		printf("♟");	break;
			case ROOK:		printf("♜");	break;
			case KNIGHT:	printf("♞");	break;
			case BISHOP:	printf("♝");	break;
			case QUEEN:		printf("♛");	break;
			case KING: 		printf("♚");	break;
		}
		// } else {
		// 	printf("%s", color_black);
		// 	switch(p->shape) {
		// 		case PAWN:		printf("♙");	break;
		// 		case ROOK:		printf("♖");	break;
		// 		case KNIGHT:	printf("♘");	break;
		// 		case BISHOP:	printf("♗");	break;
		// 		case QUEEN:		printf("♕");	break;
		// 		case KING: 		printf("♔");	break;
		// 	}
		// }
		#else
		if (p->color == WHITE) {
			switch(p->shape) {
				case PAWN:		printf(" p ");	break;
				case ROOK:		printf(" R ");	break;
				case KNIGHT:	printf(" N ");	break;
				case BISHOP:	printf(" B ");	break;
				case QUEEN:		printf(" Q ");	break;
				case KING: 		printf(" K ");	break;
			}
		} else {
			switch(p->shape) {
				case PAWN:		printf(".p.");	break;
				case ROOK:		printf(".R.");	break;
				case KNIGHT:	printf(".N.");	break;
				case BISHOP:	printf(".B.");	break;
				case QUEEN:		printf(".Q.");	break;
				case KING: 		printf(".K.");	break;
			}
		}
		#endif
	}
}
