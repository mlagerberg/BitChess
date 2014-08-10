
#include <stdbool.h>
#include <stdint.h>
#include "common.h"

/**
 * datatypes.h
 * 
 * Defines common data structures and constants for
 * pieces, boards, moves, etc.
 *
 */

#ifndef WHITE

#define WHITE 1
#define BLACK -1

#define EMPTY -1
#define PAWN 0
#define ROOK 1
#define KNIGHT 2
#define BISHOP 3
#define QUEEN 4
#define KING 5

// Game states
#define UNFINISHED 0
#define WHITE_WINS 1
#define BLACK_WINS 2
#define STALE_MATE 3
#define DRAW 4

#define FILE_A 0
#define FILE_B 1
#define FILE_C 2
#define FILE_D 3
#define FILE_E 4
#define FILE_F 5
#define FILE_G 6
#define FILE_H 7

#define RANK_1 7
#define RANK_2 6
#define RANK_3 5
#define RANK_4 4
#define RANK_5 3
#define RANK_6 2
#define RANK_7 1
#define RANK_8 0


/**
 * Simple representation of a piece of the chess set, defined
 * with a color and shape.
 * The symbol is the character used when displaying the piece.
 * Color is either BLACK or WHITE.
 * Shape is either PAWN, KNIGHT, BISHOP, ROOK, QUEEN or KING
 */
typedef struct Piece {
	int color;
	int shape;
	char *symbol;
} Piece;

/**
 * A captured piece, not used in any calculations
 * but only for gameplay purposes such as displaying
 * the captured pieces next to the board
 */
typedef struct Capture {
	/// The captured piece
	Piece *piece;
	/// The next piece in the list, or NULL
	struct Capture *next_sibling;
} Capture;

/**
 * Simple wrapper type that wraps the coordinates
 * of a square (file and rank) together with the piece on
 * that square.
 */
typedef struct Square {
	int x;
	int y;
	Piece *piece;
	/// When creating a list of squares, this'll point to the next in line.
	struct Square *next_sibling;
} Square;

/**
 * An chess board with 8x8 fields filled with Pieces (or NULL for empty fields),
 * which also keeps the state of the game, such as move count and
 * whether or not players are still allowed to castle.
 */
typedef struct Board {

	Piece *fields[8][8];

	/// Number of half-moves completed
	uint8_t ply_count;

	/// 50-move rule:
	/// Number of half-moves played without capturing a piece or moving a pawn.
	/// If 100 such half-moves are made, either player can declare a draw.
	/// TODO this is not used yet
	uint8_t fifty_move_count;

	/// Whether or not white is still allowed to perform castling on the king's side.
	bool white_can_castle_kings_side;
	/// Whether or not white is still allowed to perform castling on the queen's side.
	bool white_can_castle_queens_side;
	/// Whether or not black is still allowed to perform castling on the king's side.
	bool black_can_castle_kings_side;
	/// Whether or not black is still allowed to perform castling on the queen's side.
	bool black_can_castle_queens_side;
	/// Whether or not (and on which file) white is allowed to perform en passant.
	uint8_t white_can_en_passant;
	/// Whether or not (and on which file) black is allowed to perform en passant.
	uint8_t black_can_en_passant;

	/// White won, black won, ongoing, draw?
	uint8_t state;

	/// Number of captured white pieces
	uint8_t captures_white_count;
	/// Number of captured black pieces
	uint8_t captures_black_count;
	/// The first of a list of captured white pieces (or NULL)
	Capture *captures_white;
	/// The first of a list of captured black pieces (or NULL)
	Capture *captures_black;
} Board;


/**
 * All properties of a move, including evaluation value.
 */
typedef struct Move {
	/// Source file and rank
	uint8_t x, y;
	/// Target file and rank
	uint8_t xx, yy;
	bool is_castling;
	bool is_en_passant;
	bool gives_check;
	bool gives_draw;
	bool gives_check_mate;
	/// `true` if the player is at check before making this move.
	bool is_evasion;
	/// Only used when pawn reaches other side of the board, e.g. QUEEN or KNIGHT
	uint8_t promotion;
	/// Fitness value of the move. The higher the better for white, the lower the better for black.
	int fitness;
	/// When generating possible moves, this'll point to the next in line.
	/// So, it does NOT point to the next (opponent's) move!
	struct Move *next_sibling;
} Move;


/**
 * The opposite of a Move.
 * When a move is performed on a Board, the resulting UndoableMove
 * can be used to revert the move and restore the Board in its original state.
 * Very useful for the AI when thinking ahead.
 */
typedef struct UndoableMove {
	uint8_t x, y;
	uint8_t xx, yy;
	uint8_t hit_y;
	Piece *hit_piece;
	bool adds_to_fifty;
	bool white_can_castle_queens_side;
	bool white_can_castle_kings_side;
	bool black_can_castle_queens_side;
	bool black_can_castle_kings_side;
	bool white_can_en_passant;
	bool black_can_en_passant;
	bool is_promotion;
	bool is_castling;
	/// If undoable moves are kept in a list, this'll point to the previous half-move
	struct UndoableMove *previous;
} UndoableMove;

#endif