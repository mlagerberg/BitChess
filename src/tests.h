
#ifndef _TESTS_H_
#define _TESTS_H_

/**
 * Makes a few moves, undoes the moves, and compare the result to the initial state
 */
int test_moves();

/**
 * Checks if the validator returns the correct number of valid moves
 * for a couple of positions
 */
int test_validator();

/**
 * Writes a random board to file and reads it, compares the result.
 */
int test_serializer(char *filename);

/**
 * Makes the engine perform a couple of moves in succession
 */
int test_engine();

/**
 * Prints the fields that give check.
 * Not really a unit test, requires manually checking the output.
 */
void test_check(int player);

/**
 * Evaluates the position of a random board, and shows its reasoning.
 */
void test_evaluation();

#endif
