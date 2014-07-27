#include <stdbool.h>
#include "common.h"
#include "datatypes.h"

/**
 * piece.h / piece.c
 *
 * Some functions to create and modify Piece instances.
 *
 */
#ifndef _PIECE_H_
#define _PIECE_H_

/**
 * Creates a Piece instance of given shape and color
 */
Piece *Piece_create(int shape, int color);

Piece *Piece_clone(Piece *src);

/**
 * Creates a Piece instance by its symbol, e.g. 'bN' for a black Knight.
 */
Piece *Piece_parse(char *symbol);

void Piece_destroy(Piece *p);

/**
 * Returns the short name or symbol of a Piece, e.g.
 * 'bN' for black Knight.
 */
char *Piece_short_name(Piece *p);

/**
 * Returns true if the piece matches certain shape and color.
 */
bool Piece_matches(Piece *p, int shape, int color);

/**
 * Returns true if the two pieces are of the same shape and color.
 */
bool Piece_equals(Piece *left, Piece *right);

/**
 * Prints a piece to the console, using padding (spaces)
 * and (if possible) unicode symbols.
 * Pass NULL to display an empty field.
 */
void Piece_print(Piece *p);

#endif