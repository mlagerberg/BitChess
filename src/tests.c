#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "tests.h"
#include "debug.h"
#include "engine/datatypes.h"
#include "engine/board.h"
#include "engine/files.h"
#include "engine/piece.h"
#include "engine/move.h"

int test_serializer(char *filename) {
	int i;
	for (i = 0; i < 1; i++) {
		Board *b = debug_generate_random();
		Board_save(b, filename);
		Board *b2 = Board_read(filename);
		if (!Board_equals(false, b, b2)) {
			Board_print(b, WHITE);
			Board_print(b2, WHITE);
			printf("Test serializer: fail!\n");
			Board_destroy(b);
			Board_destroy(b2);
			return false;
		}
		Board_destroy(b);
		Board_destroy(b2);
	}

	printf("Test serializer: ok\n");
	return true;
}

int test_engine() {
	Board *b = Board_create();
	Stats stats = {0, 0, 0};
	int i=0;
	int player = WHITE;
	Move *move;
	for (i = 0; i < 6; i++) {
		move = Engine_turn(b, &stats, player, 3, true);
		Board_do_move(b, move);
		player = -player;
	}
	Board_destroy(b);
	printf("Test engine: ok\n");
	return true;
}

void test_check(int player) {
	Board *b = debug_generate_random();
	printf("White at check at fields:\n");
	debug_print_check(b, WHITE);
	pause();
	printf("Black at check at fields:\n");
	debug_print_check(b, BLACK);
	Board_destroy(b);
}


int test_validator() {
	int ok = true;
	Board *b = Board_create();

	// Clear white pieces
	int i,j;
	for (i = 0; i < 8; i++) {
		for (j = 6; j < 8; j++) {
			Board_remove_piece(b,i,j);
		}
	}
	Board_set(b, FILE_A, RANK_1, Piece_create(KING, WHITE));

	int vm_count[] = {
		3 + 1, // PAWN
		3 + 13, // ROOK
		3 + 8, // KNIGHT
		3 + 12, // BISHOP
		3 + 25, // QUEEN
		3 + 8, 	// KING
	};
	for (i = PAWN; i <= KING; i++) {
		int count;
		Move *head = Move_alloc();
		// Put a single piece back at the center
		Board_set(b, FILE_E, RANK_4, Piece_create(i, WHITE));
		// Get all valid moves:
		count = v_get_all_valid_moves_for_color(&head, b, WHITE);
		Move_destroy(head);
		if (count != vm_count[i]) {
			printf("Test valid moves: fail! Piece %d should have %d valid move(s) but has %d.\n", i, vm_count[i], count);
			ok = false;
			break;
		}
		Board_remove_piece(b, FILE_E, RANK_4);
	}
	Board_destroy(b);
	if (ok) {
		printf("Test valid moves: ok\n");
	}
	return ok;
}


int test_moves() {
	Board *b, *backup;
	Move *m[5];
	UndoableMove *um = NULL;
	int ok;
	b = Board_create();
	backup = Board_create();

	// Prepare castling
	m[0] = Move_create(WHITE, FILE_E, RANK_2, FILE_E, RANK_3, 0);
	m[1] = Move_create(WHITE, FILE_F, RANK_1, FILE_B, RANK_5, 0);
	m[2] = Move_create(WHITE, FILE_G, RANK_1, FILE_H, RANK_3, 0);
	// Castling
	m[3] = Move_create(WHITE, FILE_E, RANK_1, FILE_G, RANK_1, 0);
	// Capture
	m[4] = Move_create(WHITE, FILE_B, RANK_5, FILE_D, RANK_7, 0);

	// Do the moves
	int MOVES = sizeof(m) / sizeof(m[0]);
	int i;
	for(i = 0; i < MOVES; i++) {
		UndoableMove *undo = Board_do_move(b, m[i]);
		if (um != NULL) {
			undo->previous = um;
		}
		um = undo;
	}

	// Undo
	while (i > 0) {
		Board_undo_move(b, um);
		um = um->previous;
		i--;
	}

	// Check
	ok = Board_equals(true, b, backup);

	printf("Test move and un-move: %s\n", ok ? "ok" : "fail");
	if (!ok) {
		Board_print(backup, WHITE);
		Board_print(b, WHITE);
	}

	// Cleanup
	for(i = 0; i < MOVES; i++) {
		Move_destroy(m[i]);
	}
	Board_destroy(b);

	return ok;
}

int test_evaluation() {
	//Board *b = debug_generate_random();
	char* testfile = "./testgames/test1";
	int expected_result_1 = 8;
	Board *b = Board_read(testfile);
	printf("Test evaluation on board 1:\n");
	Board_print(b, WHITE);
	printf("Evaluation:\n");
	int value1 = Board_evaluate(b);
	printf("%d\n", value1);
	Board_destroy(b);

	testfile = "./testgames/test2";
	int expected_result_2 = 1024;
	b = Board_read(testfile);
	printf("Test evaluation on board 2:\n");
	Board_print(b, WHITE);
	printf("Evaluation:\n");
	int value2 = Board_evaluate(b);
	printf("%d\n", value2);
	Board_destroy(b);

	return value1 == expected_result_1 && value2 == expected_result_2;
}
