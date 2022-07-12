#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "color.h"
#include "common.h"
#include "datatypes.h"
#include "board.h"
#include "piece.h"
#include "validator.h"
#include "fitness.h"

// To add a random value to the final evaluation result.
//#define RANDOM_FACTOR 10

// Several penalty or reward values
const static int DOUBLE_PAWN_PENALTY = -20;			/// Two pawns of player in the same file
const static int E_AND_D_PENALTY = -10;				/// A pawn in E or D being blocked
const static int E_AND_D_BLOCKEDPENALTY = -15;		/// A pawn in E or D being blocked by opponent
const static int PAWN_NEAR_KING_BONUS = 10;			/// Pawn within 2 fields of King (manhattan distance)
const static int KNIGHT_KING_DIST_PER_TILE = -1;	/// Distance of Knight from King (manhattan distance)
const static int BISHOP_MIN_MOB_BONUS = -4;			/// For bishops with 3 or less fields available
const static int BISHOP_MAX_MOB_BONUS = 18;			/// For bishops with 12 or more fields available
const static int ROOK_MIN_MOB_BONUS = 0;			/// For rooks with 3 or less fields available
const static int ROOK_MAX_MOB_BONUS = 20;			/// For rooks with 12 or more fields available
const static int ROOK_NO_FRIENDLY_PAWNS_BONUS = 10;	/// For rooks without friendly pawns in the same file
const static int ROOK_NO_ENEMY_PAWNS_BONUS = 4;		/// For rooks without enemy pawns in the same file
const static int QUEEN_KING_DIST_PER_TILE = -1;		/// Manhattan distance between Queen and King, bonus per tile

/// Material values of the pieces
const static int MATERIAL_VALUE[5] = {100,520,330,330,980};
/// Penalties for isolated pawns
const static int ISO_PENALTY[8] = {-12,-14,-16,-20,-20,-16,-14,-12};
/// Knights get rewarded when close to the center,
/// the index is the distance in tiles to the four center squares
const static int KNIGHT_CENTER_BONUS[7] = {30,25,20,15,10,5,0};
/// King get rewarded or fined when close to the center,
/// depending on the stage of the game.
/// The index represents progress (7 being the end game, 0 being opening)
/// There's no bonus when opponent has 1 or more pawns in 3 files around king.
const static int KING_CENTER_BONUS[8] = {36,24,16,8,0,-8,-16,-24};


extern inline int max(int a, int b);

extern inline int min(int a, int b);

extern inline int get_pawn_count(int cache_pawn_count[2][8], int file, int color);

void Fitness_debug(char *message, int value, int total) {
	printf("%s%s:\t%s%d \t= %s%d%s\n", color_black, message, color_white, value, cyan, total, resetcolor);
}

int Fitness_calculate(Board *board) {
	// Cache of the number of pawns for each player in each file
	int cache_pawn_count[2][8];
	// Cache of the positions of both kings
	int kings_pos[2][2];
	// Head count
	int head_count[2];
	memset(cache_pawn_count, 0, sizeof(cache_pawn_count[0][0]) * 2 * 8);
	memset(kings_pos, 0, sizeof(kings_pos[0][0]) * 2 * 2);
	memset(head_count, 0, sizeof(head_count));

	int i, j;
	// Collect in which columns and rows are pawns,
	// and where the king is. Also, headcount.
	Piece *piece;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			piece = Board_get_piece(board, i, j);
			if (piece == NULL) {
				continue;
			}
			if (piece->shape == PAWN) {
				if (piece->color == BLACK) {
					cache_pawn_count[0][i]++;
				} else {
					cache_pawn_count[1][i]++;
				}
			} else if (piece->shape == KING) {
				if (piece->color == BLACK) {
					kings_pos[0][0] = i;
					kings_pos[0][1] = j;
				} else {
					kings_pos[1][0] = i;
					kings_pos[1][1] = j;
				}
			}
			if (piece->color == BLACK) {
				head_count[0]++;
			} else {
				head_count[1]++;
			}
		}
	}
	#if PRINT_EVAL
	Fitness_debug("head count white", head_count[1], 0);
	Fitness_debug("head count black", head_count[0], 0);
	#endif
	assert (head_count[0] <= 16 && head_count[1] <= 16);
	// Now, start analysis
	int result = 0;
	// Determine if we're in middle game or end game:
	// TODO: enhance!
	//bool endGame = ((head_count[0] + head_count[1]) <= 9)
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			piece = Board_get_piece(board, i, j);
			if (piece == NULL) {
				continue;
			}
			if (piece->shape == PAWN) {
				result += piece->color * MATERIAL_VALUE[PAWN];
				#ifdef PRINT_EVAL
				Fitness_debug("pawn\t\t", piece->color * MATERIAL_VALUE[PAWN], result);
				#endif
				// Check for isolated (= badly defended) pawns
				if (get_pawn_count(cache_pawn_count, i-1, piece->color) == 0 && get_pawn_count(cache_pawn_count, i+1, piece->color) == 0) {
					result += piece->color * ISO_PENALTY[i];
					#ifdef PRINT_EVAL
					Fitness_debug("  bad defense", piece->color * ISO_PENALTY[i], result);
					#endif
				}
				// Check for doubled pawns (= obstruction and bad defense)
				if (get_pawn_count(cache_pawn_count, i, piece->color) > 1) {
					result += piece->color * DOUBLE_PAWN_PENALTY;
					#ifdef PRINT_EVAL
					Fitness_debug("  doubled\t", piece->color * DOUBLE_PAWN_PENALTY, result);
					#endif
				}
				// Check for e and d pawns being blocked by opponent
				if ((i==3 || i==4) && j==(piece->color == BLACK ? 4 : 3)) {
					result += piece->color * E_AND_D_PENALTY;
					#ifdef PRINT_EVAL
					Fitness_debug("  e&d\t", piece->color * E_AND_D_PENALTY, result);
					#endif
					if (Board_is_color(board, i, j - piece->color, -piece->color)) {
						result += piece->color * E_AND_D_BLOCKEDPENALTY;
						#ifdef PRINT_EVAL
						Fitness_debug("  e&d blocked", piece->color * E_AND_D_BLOCKEDPENALTY, result);
						#endif
					}
				}
				// Reward pawns near king (within 2 tiles distance)
				if (piece->color == BLACK) {
					if (max(abs(i - kings_pos[0][0]), abs(j - kings_pos[0][1])) <= 2) {
						result += piece->color * PAWN_NEAR_KING_BONUS;
						#ifdef PRINT_EVAL
						Fitness_debug("  near king\t", piece->color * PAWN_NEAR_KING_BONUS, result);
						#endif
					}
				} else if (max(abs(i - kings_pos[1][0]), abs(j - kings_pos[1][1])) <= 2) {
					result += piece->color * PAWN_NEAR_KING_BONUS;
					#ifdef PRINT_EVAL
					Fitness_debug("  near king\t", piece->color * PAWN_NEAR_KING_BONUS, result);
					#endif
				}
			} else if (piece->shape == KNIGHT) {
				result += piece->color * MATERIAL_VALUE[KNIGHT];
				#ifdef PRINT_EVAL
				Fitness_debug("knight\t\t", piece->color * MATERIAL_VALUE[KNIGHT], result);
				#endif
				// Reward short distance to center
				int distance = 0;
				if (i == 2 || i == 5) {
					distance = 1;
				} else if (i == 1 || i == 6) {
					distance = 2;
				} else if (i == 0 || i == 7) {
					distance=3;
				}
				if (j==2 || j==5) {
					distance += 1;
				} else if (j == 1 || j == 6) {
					distance += 2;
				} else if (j == 0 || j == 7) {
					distance += 3;
				}
				result += piece->color * KNIGHT_CENTER_BONUS[distance];
				#ifdef PRINT_EVAL
				Fitness_debug("  near center\t", piece->color * KNIGHT_CENTER_BONUS[distance], result);
				#endif
				// Fine distance to either king
				distance = abs(kings_pos[0][0] - i) + abs(kings_pos[0][1] - j)
						 + abs(kings_pos[1][0] - i) + abs(kings_pos[1][1] - j);
				result += piece->color * KNIGHT_KING_DIST_PER_TILE * distance;
				#ifdef PRINT_EVAL
				Fitness_debug("  distance to kings", piece->color * KNIGHT_KING_DIST_PER_TILE * distance, result);
				#endif
			} else if (piece->shape == BISHOP) {
				result += piece->color * MATERIAL_VALUE[BISHOP];
				#ifdef PRINT_EVAL
				Fitness_debug("bishop\t\t", piece->color * MATERIAL_VALUE[BISHOP], result);
				#endif
				// Reward Bishop when mobility is high
				int mobility = v_get_rough_move_count_for_piece(board, i, j);
				int bonus = 0;
				if (mobility >= 12) {
					bonus = BISHOP_MAX_MOB_BONUS;
				} else if (mobility >= 9) {
					bonus = (2 * BISHOP_MAX_MOB_BONUS + BISHOP_MIN_MOB_BONUS) / 3;
				} else if (mobility >= 6) {
					bonus = (BISHOP_MAX_MOB_BONUS + BISHOP_MIN_MOB_BONUS) / 2;
				} else if (mobility >= 3) {
					bonus = (BISHOP_MAX_MOB_BONUS + 2 * BISHOP_MIN_MOB_BONUS) / 3;
				} else {
					bonus = BISHOP_MIN_MOB_BONUS;
				}
				result += piece->color * bonus;
				#ifdef PRINT_EVAL
				Fitness_debug("  mobility\t", piece->color * bonus, result);
				#endif
			} else if (piece->shape == ROOK) {
				result += piece->color * MATERIAL_VALUE[ROOK];
				#ifdef PRINT_EVAL
				Fitness_debug("rook\t\t", piece->color * MATERIAL_VALUE[ROOK], result);
				#endif
				// Reward Rook when mobility is high
				int mobility = v_get_rough_move_count_for_piece(board, i, j);
				int bonus = 0;
				if (mobility >= 12) {
					bonus = ROOK_MAX_MOB_BONUS;
				} else if (mobility >=9) {
					bonus = (2 * ROOK_MAX_MOB_BONUS + ROOK_MIN_MOB_BONUS) / 3;
				} else if (mobility >= 6) {
					bonus = (ROOK_MAX_MOB_BONUS + ROOK_MIN_MOB_BONUS) / 2;
				} else if (mobility >= 3) {
					bonus = (ROOK_MAX_MOB_BONUS + 2*ROOK_MIN_MOB_BONUS) / 3;
				} else {
					bonus = ROOK_MIN_MOB_BONUS;
				}
				result += piece->color * bonus;
				#ifdef PRINT_EVAL
				Fitness_debug("  mobility\t", piece->color * bonus, result);
				#endif
				// reward rook when no pawns are on the same file
				if (get_pawn_count(cache_pawn_count, i,piece->color) == 0) {
					result += piece->color * ROOK_NO_FRIENDLY_PAWNS_BONUS;
					#ifdef PRINT_EVAL
					Fitness_debug("  no friendly pawns", piece->color * ROOK_NO_FRIENDLY_PAWNS_BONUS, result);
					#endif
				}
				if (get_pawn_count(cache_pawn_count, i,-piece->color) == 0) {
					result += piece->color*ROOK_NO_ENEMY_PAWNS_BONUS;
					#ifdef PRINT_EVAL
					Fitness_debug("  no enemy pawns", piece->color * ROOK_NO_ENEMY_PAWNS_BONUS, result);
					#endif
				}
			} else if (piece->shape == QUEEN) {
				result += piece->color * MATERIAL_VALUE[QUEEN];
					#ifdef PRINT_EVAL
					Fitness_debug("queen\t\t", piece->color * MATERIAL_VALUE[QUEEN], result);
					#endif
				// Fine for distance to own King
				if (piece->color == BLACK) {
					result += piece->color
							* QUEEN_KING_DIST_PER_TILE
							* (abs(kings_pos[0][0] - i) + abs(kings_pos[0][1] - j));
					#ifdef PRINT_EVAL
					Fitness_debug("  distance to king", piece->color * QUEEN_KING_DIST_PER_TILE * (abs(kings_pos[0][0] - i) + abs(kings_pos[0][1] - j)), result);
					#endif
				} else {
					result += piece->color
							* QUEEN_KING_DIST_PER_TILE
							* (abs(kings_pos[1][0] - i) + abs(kings_pos[1][1] - j));
					#ifdef PRINT_EVAL
					Fitness_debug("  distance to king", piece->color * QUEEN_KING_DIST_PER_TILE * (abs(kings_pos[0][0] - i) + abs(kings_pos[0][1] - j)), result);
					#endif
				}
			} else if (piece->shape == KING) {
				// Fine/reward king according to distance to center and game state.
				// King near center gets -24 penalty when game is opening and +36 when game is ending.
				// No bonus when opponent has 1 or more pawns in 3 files around king
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
				int progress = (int) max(1, head_count[piece->color == BLACK ? 0 : 1] / 2);
				int bonus = (distance / 6) * KING_CENTER_BONUS[progress - 1];
				int pawns = get_pawn_count(cache_pawn_count, i, -piece->color);
				#ifdef PRINT_EVAL
				Fitness_debug("king - game progress (.5x headcount)", progress, 0);
				Fitness_debug("king - center distance", distance, 0);
				Fitness_debug("king - resulting bonus", bonus, 0);
				#endif
				if (i > 0) {
					pawns = pawns + get_pawn_count(cache_pawn_count, i-1, -piece->color);
				}
				if (i < 7) {
					pawns = pawns + get_pawn_count(cache_pawn_count, i+1, -piece->color);
				}
				if (pawns > 1) {
					bonus = min(bonus, 0);
					#ifdef PRINT_EVAL
					Fitness_debug("  no bonus because enemy pawns are near", bonus, 0);
					#endif
				}
				result += piece->color * bonus;
				#ifdef PRINT_EVAL
				Fitness_debug("  result\t", piece->color * bonus, result);
				#endif
			}
		}
	}
	#ifdef RANDOM_FACTOR
		int random = (rand() % RANDOM_FACTOR);
		result += random;
		#ifdef PRINT_EVAL
		Fitness_debug("random\t\t", random, result);
		#endif
	#endif
	return result;
}