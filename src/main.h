#include "engine/datatypes.h"

/**
 * main.h / main.c
 * 
 * Entry part of the application.
 * Parses command line input and 
 * responds accordingly.
 *
 */
#ifndef __MAIN_H__
#define __MAIN_H__

/**
 * Prints the version of this build and copyright information.
 */
void version();
/**
 * Shows usage instructions.
 */
void usage();
/**
 * Checks if there's an ongoing game present.
 */
int has_game(int show_error);
/**
 * Backs up an active game to .game.bak
 */
void backup_game(bool whipe_clean);
/**
 * Saves the board to file.
 */
void save_game(Board *board);
/**
 * Saves a list of moves to file.
 */
void save_move(Board *board, Move *move);
/**
 * Starts a new game. If user_color is left zero, a virtual
 * coin toss termines if the user plays white or black.
 */
void new_game(int user_color);
/**
 * Restarts the current game. The players keep their current side.
 */
void restart_game();
/**
 * Users change turns. The computer player will respond with a move.
 */
void swap_sides();
/**
 * Prints the current board.
 */
void show_board(int simple);
/**
 * Evaluates the current board for WHITE or BLACK
 * (depending on who's turn it is).
 */
int evaluate();
/**
 * Prints the given move and also returns the output.
 */
char * print_move(Board *board, Move *move);
/**
 * Parses a move from a string (shorthand notation)
 * and returns the Move (or NULL if no valid move).
 */
Move *parse_move(char *str, Board *board);
/**
 * Prints a list of valid moves.
 */
void show_moves();
/**
 * Saves a game to a backup slot,
 * returns success.
 */
int backup_to_slot(int slot);
/**
 * Restores a game from a backup slot,
 * returns success.
 */
int restore_from_slot(int slot);
/**
 * Parses the game slot from the arguments,
 * returns -1 if it fails.
 */
int get_game_slot(char *arg);
/**
 * Prepares the filenames by prepending the user's home dir
 */
void prepare_filenames();

#endif
