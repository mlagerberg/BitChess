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
inline int get_pawn_count_in_file(int cache_pawn_count[2][8], int file, int color) {
	if (file < 0 || file > 7) {
		return 0;
	}
	if (color == BLACK) {
		return cache_pawn_count[0][file];
	} else {
		return cache_pawn_count[1][file];
	}
}

/**
 * Returns the distance in tiles to the center 4 squares
 */
inline int distance_to_center(int i, int j) {
    int distance = 0;
    if (i == 2 || i == 5) {
        distance = 1;
    } else if (i == 1 || i == 6) {
        distance = 2;
    } else if (i == 0 || i == 7) {
        distance = 3;
    }
    if (j == 2 || j == 5) {
        distance += 1;
    } else if (j == 1 || j == 6) {
        distance += 2;
    } else if (j == 0 || j == 7) {
        distance += 3;
    }
    return distance;
}

#endif
