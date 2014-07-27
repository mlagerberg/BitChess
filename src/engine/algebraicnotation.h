#include "common.h"
#include "datatypes.h"
#include "move.h"

/**
 * algebraicnotation.h / algebraicnotation.c
 *
 * Contains functions to read and write algebratic (shorthand) notation.
 * Parsing input is performed by basically generating a list of valid
 * moves in AN and checking if the input is in the list.
 *
 */
#ifndef _ALGEBRAICNOTATION_H_
#define _ALGEBRAICNOTATION_H_

/**
 * Creates a Move object from the given input string and given board configuration.
 * The input string must be very precise, e.g. the '+' can not be left
 * out when a move makes check.
 */
Move * AN_parse(char *str, Board *board);

/**
 * Returns the shorthand notation of the given move.
 * Parameter 'show_number' controls if the move number is included in the
 * output. If so, parameter 'complete' can be used to show an ellipsis for
 * the previous whit move in case the current move is black, e.g.:
 * 4. ... Nc5
 * With complete = false, this would be:
 * 4. Nc5
 */
char * AN_format(Board *board, Move *m, int complete, int show_number);

#endif
