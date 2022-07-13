#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "color.h"
#include "common.h"
#include "datatypes.h"
#include "engine.h"
#include "fitness.h"
#include "heuristics.h"
#include "move.h"
#include "piece.h"
#include "validator.h"

#ifdef THREADS
#include <pthread.h>
#include <unistd.h>
#endif

/**
 * Simple struct for passing a chunk of moves
 * to a thread
 */
typedef struct ThreadData {
	Board *board;	/// Board position
	Stats *stats;	/// Output: chunk statistics
	int color;		/// Who's turn it is
	int ply_depth;	/// Max ply depth
	Move **head;	/// List of moves
	int from;		/// Index of move to start at (in **head)
	int to;			/// Index+1 of move to stop at (in **head)
} ThreadData;

/**
 * Prints an indent suitable for the current search depth.
 * Used when printing a tree of available moves and counter moves.
 */
#ifdef PRINT_ALL_MOVES
static void print_depth(int depth) {
	if (depth == 1) {
		printf("\n.   .   ");
	} else if (depth == 2) {
		printf("\n.   ");
	} else if (depth == 3) {
		printf("\n.");
	} else {
		printf("\n");
	}
}
#endif

/**
 * Returns the best move from the given list of moves. The given list
 * of moves <strong>must be usable as an array</strong>, like so:
 * 	head[0]->fitness = 0; head[1]->fitness = 0; // etc.
 * <strong>So a list of individual moves linkes together by their ->next_sibling is NOT ok.</strong>
 * The length of the list must be specified in the last parameter.
 * 
 * The list of moves is send in chunks to evaluate_moves after which the
 * move with best value is picked (or randomly one of the moves within 10% 'distance'
 * from the best move).
 */
static Move *get_best_move(Board *board, Stats *stats, int color, int ply_depth, Move **head, int total);

/**
 * Evaluates given moves by calling the alpha-beta algorithm on each of them.
 * Only meant to be used at the root search depth (alpha-beta does the rest).
 */
 void *evaluate_moves(void *data);

/**
 * Core of the algorithm: a recursive method that implements the alpha-beta algorithm.
 * Thinks ahead with the given ply depth, returns immediately if depth==0.
 *
 * Parameters:
 * - *board 	- the board to perform a move on. Must be reset to it's original state before finishing.
 * - *stats 	- performance stats, will be adjusted.
 * - dist 		- distance from root. Like the reverse of depth
 * - depth 		- remaining ply depth. When combined with extra_depth this is below minimum,
 * 				  the search will not branch further.
 * - extra depth- when quiescence score determines that a deeper search is reguired,
 *				  the value of this paramter is increased.
 * - quiescence_score
 * 				- indicates if the current branch is 'quiet' or not. When this value is above a
 *                certain threshold the branch is considered volatile and will be
 *  			  searched a bit deeper.
 * - alpha 		- alpha cutoff value
 * - beta 		- beta cutoff value
 * - color 		- current turn
 * - *killers	- array with pointers to killer moves.
 *
 * Returns an array: {fitness, state} where fitness is the eventual board position value
 * and state is either 1 if the move results directly in check mate, 2 if it directly
 * results in stale mate, and 0 otherwise.
 */
static int * alpha_beta(Board *board, Stats *stats, int dist, int depth, int extra_depth, int quiescence_score, int alpha, int beta, int color, unsigned int killers[]);

/**
 * Creates an array of pointers to the moves, so that
 * they can be accessed as: *first, *(first+1), *(first+2), ... *(first+total-1).
 * Or as *first, *first[1], etc., I think.
 * The array is shuffled and put into the first parameter.
 */
static void create_shuffled_array(Move **out, Move *head, int total);

/**
 * Wether or not to print progress.
 * Set by Engine_turn and used by several other methods.
 */
static int draw_progress = true;


Move *Engine_turn(Board *board, Stats *stats, int color, int ply_depth, int verbosity) {
	clock_t start_time, stop_time;
	start_time = clock();
	// Housekeeping
	draw_progress = verbosity != 0;
	if (verbosity >= 2) {
		printf("Thinking");
		#ifdef PRINT_THINKING
		printf("\n");
		#endif
	}
	// Init randomizer:
	srand(time(NULL));
	// Generate list of all valid moves:
	Move *head = Move_alloc();
	int total = v_get_all_valid_moves_for_color(&head, board, color);
	// No reason to evaluate forced moves:
	if (total == 1) {
		if (PRINT_STATS || verbosity > 1) {
			printf("Only one move possible, no moves evaluated.\n");
		}
		return head;
	}
	// Make array and shuffle it:
	Move **arr = malloc(sizeof(Move) * total);
	create_shuffled_array(arr, head, total);
	// Find the best move:
	head = get_best_move(board, stats, color, ply_depth, arr, total);
	if (PRINT_STATS || verbosity > 1) {
		stop_time = clock();
		double duration = ((double) (stop_time - start_time)) / CLOCKS_PER_SEC;
		duration /= MAX_THREADS;
		printf("\nEvaluated %d positions and %d moves in %.2f seconds.\n",
			stats->boards_evaluated, stats->moves_count,
			duration);
	}
	// Make a copy, so the rest can easily be destroyed in 1 go:
	Move *result = Move_clone(head);
	Move_destroy(arr[0]);
	return result;
}


static Move *get_best_move(Board *board, Stats *stats, int color, int ply_depth, Move **head, int total) {
	int i;
	int threads;
// If threading is disabled, we use 0 threads, obviously
#ifdef THREADS
	threads = MAX_THREADS;
#else
	threads = 0;
#endif

	if (threads == 0) {
		// No threading
		ThreadData data;
		data.board = board;
		data.stats = stats;
		data.color = color;
		data.ply_depth = ply_depth;
		data.head = head;
		data.from = 0;
		data.to = total - 1;
		evaluate_moves(&data);
 	}

// preprocessor toggle instead of if-statement because this code block
// contains code that Windows doesn't support.
#ifdef THREADS 
 	else {
		// Divide array in chunks and evaluate moves in parallel.
		int chunks = min(threads, max(1, total/2));
		ThreadData data[chunks];
		pthread_t thread[chunks];
	    // Rounding up the number of tasks per thread, which is useful
	    // in case total does not divide evenly by chunks.
		int chunk_size = (total + chunks - 1) / chunks;
		// Divide work
		for (i = 0; i < chunks; i++) {
			data[i].board = Board_clone(board);
			data[i].stats = calloc(0, sizeof(Stats));
			data[i].color = color;
			data[i].ply_depth = ply_depth;
			data[i].head = head;
			data[i].from = i * chunk_size;
			data[i].to = (i+1) * chunk_size;
		}
		// Last one must not go past the end:
		data[chunks-1].to = total;
		// Do each chunk
	    for (i = 0; i < chunks; i++) {
	        // Launch thread
	        pthread_create(&thread[i], NULL, evaluate_moves, (void *) &data[i]);
	    }
	    // Wait for threads to finish
	    for (i = 0; i < chunks; i++) {
	        pthread_join(thread[i], NULL);
	    }
	    // Combine statistics
	    for (i = 0; i < chunks; i++) {
	        stats->moves_count += data[i].stats->moves_count;        
	        stats->boards_evaluated += data[i].stats->boards_evaluated;
	        // Board_destroy destroys the pieces as well, so:
	        Board_destroy(data[i].board);
	    }
	}
#endif

	// Find the highest item:
	int white = (color == WHITE);
	int best = 0;
	for(i = 1; i < total - 1; i++) {
		if (white == (head[i]->fitness > head[best]->fitness)) {
			best = i;
		}
	}
	return head[best];
}


/**
 * Root level of the search.
 * Engine_turn calls this function on a list of moves (by proxy of get_best_move,
 * which handles multithreading and picking the highest value from the evaluations.
 * This method in turn calls alpha_beta for the next search depth.
 */
void *evaluate_moves(void *threadarg) {
	ThreadData *data = (ThreadData *) threadarg;
	bool white = (data->color == WHITE);
	int i;
	unsigned int killers[MAX_PLY_DEPTH + MAX_EXTRA_PLY_DEPTH] = { 0 };

	#ifdef PRINT_THINKING
	int best_fitness = white ? MIN_FITNESS : MAX_FITNESS;
	#endif

	#ifdef PRINT_MOVES
	printf("Evaluating chunk from %d to %d of these moves:\n", data->from, data->to);
	#endif

	int alpha = MIN_FITNESS;
	int beta = MAX_FITNESS;
	// Try each move in the chunk
	for (i = data->from; i < data->to; i++) {
		Move *move = data->head[i];
		#ifdef PRINT_MOVES
			printf("\n[%d-%d:%d] %s%c%d-%c%d%s\n",
				data->from, data->to, i,
				white ? color_white : color_black,
				move->x + 'a', 8 - move->y,
				move->xx + 'a', 8 - move->yy,
				resetcolor);
		#else
			#ifdef PRINT_ALL_MOVES
			printf("\n");
			Move_print_color(move, data->color);
			printf(" >");
			#endif
		#endif
		// Perform the move
		UndoableMove *umove = Board_do_move(data->board, move);
		// Recurse!
		int * ab = alpha_beta(
				data->board,
				data->stats,
				1,
				data->ply_depth-1,
				0, 0,
				alpha, beta,
				-data->color,
				killers);

		move->fitness = ab[0];
		if (ab[1] == WHITE_WINS || ab[1] == BLACK_WINS) {
			move->gives_check_mate = true;
		} else if (ab[1] == STALE_MATE) {
			move->gives_draw = true;
		}

		#ifdef PRINT_ALL_MOVES
			printf("\n");
			Move_print_color(move, data->color);
			printf(" %s< %d%s", white ? color_white : color_black, move->fitness, resetcolor);			
		#endif

		#ifdef PRINT_THINKING
			// if (white) {
			// 	printf("Is fitness %d > %d for ", move->fitness, best_fitness);
			// } else {
			// 	printf("Is fitness %d < %d for ", move->fitness, best_fitness);
			// }
			// Move_print(move);
			// printf("\n");
			if ((white && move->fitness > best_fitness) || (!white && move->fitness < best_fitness)) {
				printf("  Considering ");
				Move_print_color(move, data->color);
				printf(", evaluation: %d beats %d\n", move->fitness, best_fitness);
				best_fitness = move->fitness;
			}
		#else
			if (draw_progress) {
				printf(".");
				fflush(stdout);
			}
		#endif

		// Restore the board
		Board_undo_move(data->board, umove);
		Undo_destroy(umove);

		// Check for alpha/beta cut-offs
		if (white) {
			if (move->fitness > alpha) {
				alpha = move->fitness;
			}
			if (alpha >= beta) {
				break;
			}
		} else {
			if (move->fitness < beta) {
				beta = move->fitness;
			}
			if (alpha >= beta) {
				break;
			}
		}
	}
	return NULL;
}


static int * alpha_beta(Board *board, Stats *stats, int dist, int depth, int extra_depth, int quiescence_score, int alpha, int beta, int color, unsigned int killers[]) {
	static int result[2];

	stats->moves_count++;
	// Stop when at maximum search depth
	if (depth + extra_depth <= MIN_PLY_DEPTH_REMAINDER) {
		// No need to go beyond MIN_PLY_DEPTH if move is 'quiet':
		bool allow_pruning = (quiescence_score < QUIESCENCE_THRESHOLD);
		if (allow_pruning || depth + extra_depth <= 0) {
			stats->boards_evaluated++;
			result[0] = Board_evaluate(board);
			result[1] = 0;
			#ifdef PRINT_ALL_MOVES
				printf(" %s%d%s", WHITE ? color_white : color_black, result[0], resetcolor);
			#endif
			return result;
		}
	}

	bool at_check = v_king_at_check(board, color);
	Move *moves = Move_alloc();
	v_get_all_valid_moves_for_color(&moves, board, color);
	if (moves == NULL || Move_is_nullmove(moves)) {
		Move_destroy(moves);
		if (at_check) {
			// Mate!
			if (color == WHITE) {
				result[0] = MIN_FITNESS;
				result[1] = WHITE_WINS;
			} else {
				result[0] = MAX_FITNESS;
				result[1] = BLACK_WINS;
			}
		} else {
			// Stalemate!
			result[0] = 0;
			result[1] = STALE_MATE;
		}
		return result;
	}

	// Sort the killer move to the front
	Heuristics_reorder(killers, dist, &moves);

	// Important not to count too old events:
	quiescence_score /= 2;
	Move *curr = moves;
	while (curr) {
		#ifdef PRINT_ALL_MOVES
			print_depth(depth);
			Move_print_color(curr, color);
			printf(" >");
		#endif

		UndoableMove *umove = Board_do_move(board, curr);
		int score = Move_quiescence(umove, board);
		int * ab = alpha_beta(
				board,
				stats,
				dist + 1,
				depth - 1,
				at_check && extra_depth < MAX_EXTRA_PLY_DEPTH ? extra_depth + 1 : extra_depth,
				quiescence_score + score,
				alpha,
				beta,
				-color,
				killers);
		if (ab[1] == WHITE_WINS || ab[1] == BLACK_WINS) {
			curr->gives_check_mate = true;
		} else if (ab[1] == STALE_MATE) {
			curr->gives_draw = true;
		}
		int result = ab[0];
		Board_undo_move(board, umove);
		Undo_destroy(umove);
		#ifdef PRINT_ALL_MOVES
			if (depth > 1) {
				print_depth(depth);
				curr->fitness = result;
				Move_print_color(curr, color);
				printf(" %s< %d%s", color==WHITE ? color_white : color_black, result, resetcolor);
			}
		#endif

		// The crux of the alpha-beta algorithm:
		// Breaking out of the loop if a certain minimum value
		// of a branch is already higher than the maximum of another
		if (color == WHITE) {
			if (result > alpha) {
				alpha = result;
			}
			if (alpha >= beta) {
				//printf("%s*%s", red, resetcolor);
				Heuristics_produced_cutoff(killers, dist, curr);
				break;
			}
		} else {
			if (result < beta) {
				beta = result;
			}
			if (alpha >= beta) {
				//printf("%s*%s", red, resetcolor);
				Heuristics_produced_cutoff(killers, dist, curr);
				break;
			}
		}
		curr = curr->next_sibling;
	}

	Move_destroy(moves);
	if (color == WHITE) {
		result[0] = alpha;
	} else {
		result[0] = beta;
	}
	result[1] = UNFINISHED;
	return result;
}


static void create_shuffled_array(Move **arr, Move *head, int total) {
	Move *curr = head;
	int i;
	// Put all pointers in array:
	for (i = 0; i < total; i++) {
		arr[i] = curr;
		if (curr->next_sibling) {
			curr = curr->next_sibling;
		} else if (i+1 < total) {
			fprintf(stderr, "List of moves ends at %d while length should be %d", i+1, total);
			break;
		}
	}

	#ifndef DEBUG_KEEP_MOVES_SORTED
		// Shuffle array by randomly swapping elements
		for (i = 0; i < total; i++) {
			// Swap move at i with move at a random position
			int target = rand() % total;
			if (target != i) {
				Move *temp = arr[i];
				arr[i] = arr[target];
				arr[target] = temp;
			}
		}

		// Restore the ->next_sibling pointers to the new order
		if (total > 1) {
			for (i = 0; i < total - 1; i++) {
				arr[i]->next_sibling = arr[i+1];
			}
			arr[total-1]->next_sibling = NULL;
		}
	#endif
}
