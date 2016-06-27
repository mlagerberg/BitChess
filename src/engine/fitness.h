#include "datatypes.h"

/**
 * fitness.h / fitness.c
 *
 * Contains functions to evaluate how good a certain board position is for
 * either side.
 *
 */
#ifndef _FITNESS_H_
#define _FITNESS_H_


/**
 * Evaluates a board position to see which player is doing better and how much better.
 * Besides material values, included in the evaluation are rewards and penalties regarding
 * doubled and isolated pawns, rook and bishop mobility, distance of knights to the center,
 * and some other rules.
 *
 * Some research is probably needed on this method to improve the AI performance.
 * Some sources suggest that a simpler, quicker evaluation method might be beneficial
 * if it allows the engine to search a level deeper.
 *
 * Returns negative when position is in favor of black, positive when white is
 * ahead and zero when neither side has an advantage.
 */
int Fitness_calculate(Board *board);

inline int max(int a, int b) {
	return a > b ? a : b;
}

inline int min(int a, int b) {
	return a < b ? a : b;
}

/**
 * Returns the number of pawns of the given color in the given file,
 * by looking it up in the given lookup table.
 */
inline int get_pawn_count(int cache_pawn_count[2][8], int file, int color) {
	if (file < 0 || file > 7) {
		return 0;
	}
	if (color == BLACK) {
		return cache_pawn_count[0][file];
	} else {
		return cache_pawn_count[1][file];
	}
}

#endif