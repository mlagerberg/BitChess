#include <stdbool.h>
#include "datatypes.h"

/**
 * heuristics.h / heuristics.c
 *
 * Functions to determine heuristics, used in Move-Ordering.
 *
 */
#ifndef _HEURISTICS_H_
#define _HEURISTICS_H_

/**
 * Tells the Heuristics calculated that this move
 * has produced a beta-cutoff in the search tree,
 * which is a sign of a positive heuristic.
 */
void Heuristics_produced_cutoff(unsigned int killers[], int depth, Move *m);

/**
 * Puts the killer move at the front.
 * The killer move is recognized by having a higher heuristic.
 */
void Heuristics_reorder(unsigned int killers[], int depth, Move **head);

#endif