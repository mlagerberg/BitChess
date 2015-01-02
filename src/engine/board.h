#include <stdbool.h>
#include "common.h"
#include "datatypes.h"

/**
 * board.h / board.c
 * 
 * Representation of a board. Includes lists
 * of Pieces, lists of captured pieces, etc.
 * The functions here allow to modify, save,
 * load and show the board.
 *
 * Check datatypes.h for the struct Board that
 * is the actual datastructure of a board.
 *
 */

#ifndef _BOARD_H_
#define _BOARD_H_

/**
 * Creates a new board
 */
Board *Board_create();

/**
 * Copy constructor. Makes a deep copy
 * of the pieces as well.
 */
Board *Board_clone(Board *src);

/**
 * Frees the memory used by the board.
 * ALSO destroys all Pieces on it!
 */
void Board_destroy(Board *b);

/**
 * Resets the board to it's starting position.
 * All data concerning the current game status is lost.
 */
void Board_reset(Board *b);

/**
 * Removes a piece from the board.
 */
void Board_remove_piece(Board *b, int x, int y);

/**
 * Outputs a board to the console, from
 * the viewpoint of the given player
 */
void Board_print(Board *b, int player);

/**
 * Like Board_print(Board*, int), but in color
 */
void Board_print_color(Board *b, int player);

/**
 * Prints a list of captures piece from the given player.
 */
void Board_print_captures(Board *b, int color);

/**
* Returns the piece on square pos=(x,y).
* Both x and y must lie within the interval [0,7].
* If the parameters can be outside this range, use getPieceSafe() instead. 
*/
inline Piece *Board_get_piece(Board *b, int x, int y);

/**
* Equal to getPiece, but with the difference that this
* method checks if the specified square is on the board.
* If not, this method returns an empty Piece object.
* If it is priorly known that the specified square is on the board,
* use getPiece instead for performance.
*/
inline Piece *Board_get_piece_safe(Board *b, int x, int y);

/**
* Returns true if there's no piece on square pos=(x,y).
* Both x and y must lie within the interval [0,7].
*/
inline bool Board_is_empty(Board *b, int x, int y);

/**
 * Returns the value assigned to this board position
 */
inline int Board_evaluate(Board *b);

/**
* Checks if the given tile contains the given color and type.
* Usefull to skip having to check if the tile is empty.
* Does NOT check if the tile is within the boundaries of the board.
* Use is_at_safe for that.
*/
inline bool Board_is_at(Board *b, int x, int y, int shape, int color);

/**
* Checks if the given tile contains the given color and type.
* Usefull to skip having to check if the position is within
* the boundaries of the board and if the tile is empty
*/
inline bool Board_is_at_safe(Board *b, int x, int y, int shape, int color);

/**
* Checks if the given tile contains the given color.
* Usefull to skip having to check if the tile is empty.
* Does NOT check if the tile is within the boundaries of the board!
*/
inline bool Board_is_color(Board *b, int x, int y, int color);

/**
* Checks if the given tile contains the given piece type.
* Usefull to skip having to check if the tile is empty.
* Does NOT check if the tile is within the boundaries of the board!
*/
inline bool Board_is_type(Board *b, int x, int y, int shape);

/**
* Puts the given piece on position pos=(i,j) on the board.
* This method assumes that pos is a valid position.
* The piece may be NULL to put an empty field.
* <s>The given piece is cloned, so changing the piece after calling this
* method does not affect the board.</s>
* 
* - Piece is NOT cloned. Why should it?
*
* - What to do with the occupied field if ! null? Free memory? Clone result?
*/
inline void Board_set(Board *b, int x, int y, Piece *p);

/**
 * WHITE or BLACK
 */
inline int Board_turn(Board *b);

/**
* Tests if the board equals another board. Note that this method is designed to
* be used in checking whether a position occurs for the third time, so ply count
* is ignored.
* If `quick` is true, en passant and castling is also ignored.
*/
bool Board_equals(bool quick, Board *left, Board *right);


/**
* Adapts the board according to a given move.
* This function assumes that the parameter is a valid move.
* Returns an UndoableMove
*/
UndoableMove *Board_do_move(Board *board, Move *move);

/**
* Undoes a move that was done using do_move() and brings the board back to
* it's original state before that move. Parameter umove is a move
* describing how to undo a move. This object is usually  returned by do_move.
*/
void Board_undo_move(Board *board, UndoableMove *umove);

/**
 * Saves a board to a file
 */
void Board_save(Board *board, const char *filename);

/**
 * Reads a board from a file
 */
Board *Board_read(const char *filename);

/**
 * If the UndoableMove captures a piece,
 * this'll add it to the list of captured pieces.
 */
void Board_add_capture(Board *board, UndoableMove *um);

/**
 * Adds a piece to the list of captured pieces
 */
void Board_add_captured_piece(Board *board, Piece *piece);

/**
 * Destroys a Capture object along with the Piece in it.
 */
void Capture_destroy(Capture *capture);

#endif