#include "common.h"
#include "datatypes.h"

/**
 * square.h / square.c
 *
 * Some very basic functions revolving a single
 * field of a board. Defined by the Square struct,
 * it simply consists if coordinates and a reference to
 * a Piece.
 *
 */
#ifndef _SQUARE_H_
#define _SQUARE_H_

Square *Square_create(int x, int y, Board *board);

void Square_destroy(Square *s);

int Square_is_at(Square *square, int x, int y);

char *Square_coords_to_string(int x, int y);

char *Square_to_string(Square *s);

void Square_print(Square *s);

/**
 * Translates a string coordinate, e.g. "a2" to x,y positions.
 * The ->piece member will remain NULL.
 */	
Square *Square_parse(char *tile);

#endif