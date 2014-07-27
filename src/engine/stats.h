/**
 * stats.h
 *
 * Simple structure used in keeping some statistics
 * while the engine is running.
 * When thinking of a move, each thread will track the number
 * of moves tried and the number of board positions evaluated.
 * The data of all threads is then summarized into another instance.
 * Useful to test if optimizations have the desired effect.
 */

#ifndef _STATS_H_

#define _STATS_H_

typedef struct Statistics {

	/// Time stamp at which calculations have started.
	int start_time;
	/// Current number of moves examined in the current turn
	int moves_count;
	/// Number of boards evaluated in the current turn
	int boards_evaluated;	

} Stats;

#endif