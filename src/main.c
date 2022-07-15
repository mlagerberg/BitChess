#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gitversion.h"
#include "main.h"
#include "tests.h"
#include "engine/algebraicnotation.h"
#include "engine/board.h"
#include "engine/common.h"
#include "engine/datatypes.h"
#include "engine/engine.h"
#include "engine/files.h"
#include "engine/move.h"
#include "engine/piece.h"
#include "engine/simplenotation.h"
#include "engine/stats.h"
#include "engine/validator.h"
#ifdef UNICODE_FIX
#include <windows.h>
#endif


const char* APP = "BitChess";
const char* VERSION = "0.1.7";
const char* AUTHOR = "Mathijs Lagerberg";
char* DEFAULT_FILE = "game";
char* SLOT_FILE = "%i.game";
char* SLOT_MOVES_FILE = "%i.moves";
char* BACKUP_FILE = "game.bak";
char* DEFAULT_MOVES_FILE = "moves";
char* BACKUP_MOVES_FILE = "moves.bak";
char* YEAR = &__DATE__[7];


// Level of verbosity. The higher the more output
// Silenced by using -s at the cli.
static int verbosity = 2;
// If true, performs the move without having the AI player do a counter move.
// Enabled by using -x argument.
static int no_counter = 0;
// Using the -m command, this toggles between shorthand (algebraic)
// and simple move notation.
static bool algebraic = true;


// Prepares the Windows Command Prompt to show unicode characters
#ifdef UNICODE_FIX
void fix_unicode() {
	//UINT oldCodePage;
	//oldCodePage = GetConsoleOutputCP();
	if (!SetConsoleOutputCP(65001)) {
		fprintf(stderr, "Unable to set codepage to unicode.\n");
	}
}
#endif


int main(int argc, char *argv[]) {
	// Enable streaming output:
	setvbuf(stdout, NULL, _IONBF, 0);
	// Fix output codepage for windows:
	#ifdef UNICODE_FIX
	fix_unicode();
	#endif
	// Prepare filenames:
	prepare_filenames();

	// Only few arguments are allowed:
	if (argc < 2 || argc > 3) {
		usage();
		return 1;
	}

	// Silent/Helpful/No-counter-move mode?
	int index;
	if (strcmp("-s", argv[1]) == 0) {
		// Silent mode, silences output to only critical messages, such as moves.
		// Useful in case the output is to be parsed by another process which
		// does not care about "thinking..." or a unicode chess board picture.
		verbosity = 0;
		index = 2;
		if (argc != 3) {
			usage();
			return 1;
		}
	} else if (strcmp("-m", argv[1]) == 0) {
		// Machine-readible moves. When on, input moves can (and should) be inserted
		// using a simplified notation, e.g. 'a1-b3' instead of 'Nb3'.
		// More info in simplenotation.c
		verbosity = 0;
		index = 2;
		algebraic = false;
		if (argc != 3) {
			usage();
			return 1;
		}
	} else if (strcmp("-x", argv[1]) == 0) {
		// Performs the move without having the AI player do a counter move.
		index = 2;
		no_counter = 1;
		if (argc != 3) {
			usage();
			return 1;
		}
	} else if (strcmp("-mx", argv[1]) == 0 || strcmp("-xm", argv[1]) == 0) {
		// -m and -x combined
		verbosity = 0;
		no_counter = 1;
		index = 2;
		algebraic = false;
		if (argc != 3) {
			usage();
			return 1;
		}
	} else if (strcmp("-sx", argv[1]) == 0 || strcmp("-xs", argv[1]) == 0) {
		// -s and -x combined
		verbosity = 0;
		no_counter = 1;
		index = 2;
		if (argc != 3) {
			usage();
			return 1;
		}
	} else {
		// Default to max verbosity.
		verbosity = 2;
		index = 1;
	}

	if (strcmp("test", argv[index]) == 0) {
		// Run tests
		return !test_moves()
			|| !test_validator()
			|| !test_serializer("test.chess")
			|| !test_engine()
			|| !test_evaluation();
	} else if (strcmp("testeval", argv[index]) == 0) {
		// Run visual test
		return test_evaluation();
	} else if (strcmp("-v", argv[index]) == 0
			|| strcmp("version", argv[index]) == 0
			|| strcmp("--version", argv[index]) == 0) {
		// Print version string
		version();
	} else if (strcmp("-h", argv[index]) == 0
			|| strcmp("help", argv[index]) == 0
			|| strcmp("--help", argv[index]) == 0) {
		// Show help
		usage();
	} else if (strcmp("-n", argv[index]) == 0 || strcmp("new", argv[index]) == 0) {
		// Start new game, randomizing the sides
		new_game(0);
		if (verbosity > 1) {
			show_board(false);
		}
	} else if (strcmp("white", argv[index]) == 0) {
		// Hidden option to start a new game and influence faith or rig the coin toss 
		new_game(WHITE);
		if (verbosity > 1) {
			show_board(false);
		}
	} else if (strcmp("black", argv[index]) == 0) {
		// Hidden option to start a new game and influence faith or rig the coin toss 
		new_game(BLACK);
		if (verbosity > 1) {
			show_board(false);
		}
	} else if (strcmp("-r", argv[index]) == 0 || strcmp("restart", argv[index]) == 0 || strcmp("reset", argv[index]) == 0) {
		// Restart the current game from the start, keep sides.
		restart_game();
		if (verbosity > 1) {
			show_board(false);
		}
	} else if (strcmp("switch", argv[index]) == 0 || strcmp("swap", argv[index]) == 0) {
		// Switch places in current game.
		swap_sides();
		if (verbosity > 1) {
			show_board(false);
		}
	} else if (strcmp("-p", argv[index]) == 0 || strcmp("print", argv[index]) == 0) {
		// Print the current board.
		show_board(!algebraic);
	} else if (strcmp("-e", argv[index]) == 0 || strcmp("evaluate", argv[index]) == 0) {
		// Print board position value.
		evaluate();
	} else if (strcmp("list", argv[index]) == 0) {
		// Print a list of available moves:
		show_moves(false);
	} else if (strcmp("backup", argv[index]) == 0) {
		// Backup the current game to one of nine savegame slots.
		if (index + 1 >= argc) {
			fprintf(stderr, "No backup number given to save file to. Please retry and\nspecify a number 0-9.\n");
			return 1;
		}
		return backup_to_slot(get_game_slot(argv[index+1]));
	} else if (strcmp("restore", argv[index]) == 0) {
		// Restore the current game from one of nine savegame slots.
		if (index + 1 >= argc) {
			fprintf(stderr, "No backup number given to restore file from. Please retry and\nspecify a number 0-9.\n");
			return 1;
		}
		return restore_from_slot(get_game_slot(argv[index+1]));
	} else {
		// No game? Can't play.
		if (!has_game(true)) {
			fprintf(stderr, "No game present.\n");
			return 1;
		}
		// Read game from file
		Board *board = Board_read(DEFAULT_FILE);
		if (board->state != UNFINISHED) {
			Board_destroy(board);
			fprintf(stderr, "Game has finished.\n");
			return 1;
		}
		// Parse input move
		Move *move = parse_move(argv[index], board);
		if (move == NULL) {
			fprintf(stderr, "Unable to parse move, or invalid move.\n");
			Board_destroy(board);
			return 1;
		} else {
			// Save move first (must be formatted before it's executed, otherwise formatting is wrong)
			save_move(board, move);
			// Execute move
			UndoableMove *um = Board_do_move(board, move);
			// Check for captures
			Board_add_capture(board, um);
			Undo_destroy(um);
			if (verbosity > 1) {
				// Print the result
				Board_print(board, -Board_turn(board));
			}

			// Game over?
			if (move->gives_check_mate) {
				printf("player wins");
			} else if (!no_counter) {
				// Think of a counter move...
				Move *counter;
				Stats stats = {0, 0, 0};
				counter = Engine_turn(board, &stats, Board_turn(board), MAX_PLY_DEPTH, verbosity);
				// Show counter move
				print_move(board, counter);
				// print_move and save_move both execute the formatter,
				// but they use a different format so we can't optimize this code
				save_move(board, counter);
				// Execute counter move
				um = Board_do_move(board, counter);
				// Check for captures for this move too
				Board_add_capture(board, um);
				Undo_destroy(um);
				Move_destroy(counter);
			}
			// Save to file
			save_game(board);
			// Clean up
			Board_destroy(board);
			Move_destroy(move);
			if (verbosity > 1 && !(move->gives_check_mate)) {
				show_board(false);
			}
		}
	}

	return 0;
}

void version() {
	#ifdef DEBUG
		printf("%s %s-%s DEBUG (%s)\n", APP, VERSION, GIT_VERSION, __DATE__);
	#else
		printf("%s %s-%s (%s)\n", APP, VERSION, GIT_VERSION, __DATE__);
	#endif
	if (YEAR[2]=='1' && YEAR[3]=='4') {
		// FIXME not failsafe for the year 2114, 2214, etc.
		printf("Copyright %s %s.\n", YEAR, AUTHOR);
	} else {
		printf("Copyright 2014-%s %s.\n", YEAR, AUTHOR);
	}
	printf("This is free software, released under Apache 2.0 license.\n");
	printf("The software is provided as is without any guarantees or warranty.\n");
}

void usage() {
	version();
	// Hidden features:
	//	     white 			Starts a new where the user plays white.
	//	     black 			Starts a new where the user plays black.
	//		 list			Shows the user's available moves.
	//		evaluate 		Shows the board position evaluation of the current game.
	
	printf("\nusage:       chess [-s,-m,-x] <command>\n\n");
	printf("commands and options:\n");
	printf(" <move>       Make a move in an ongoing game. The computer player will respond\n");
	printf("              with a move. The move should be in a algebraic notation like so:\n");
	printf("              'Ndxe5+' (no quotes). If the move is a pawn promotion, an extra\n");
	printf("              capital letter must denote what piece to promote to, e.g.:\n");
	printf("              'e8=Q'. Castling can be performed by 'O-O' or 'O-O-O' (i.e.\n");
	printf("              capital 'oh', not zeros).\n");
	printf("  new         Starts a new game, where a virtual coin toss determines who plays\n");
	printf("              white. The game files are stored in ~/.BitChess/.\n");
	printf("  print       Shows the current board position.\n");
	printf("  reset       Restarts an ongoing game.\n");
	printf("  switch      Switches sides in an ongoing game. The computer player will make\n");
	printf("              a move.\n");
	printf("  backup <n>  Backs up the current game to a file with the specified number\n");
	printf("              <n>. Can be restored using the restore command.\n");
	printf("  restore <n> Restores a backed-up game from the file with the given number\n");
	printf("              <n>. Requires a <n>.game file to be present.\n");
	printf("-s            Silences output to only critical messages, such as moves.\n");
	printf("-m            Uses simple move notation. When used, moves must be written like\n");
	printf("              so: 'd7-d5'. No indications for pieces, captures or check are\n");
	printf("              used. For pawn promotions, append an extra capital letter:\n");
	printf("              'e7-e4Q'. Automatically also enables silent mode, i.e.: -s.\n");
	printf("-x            Performs the move without having the AI player do a counter move.\n");
	printf("-h --help     Shows this help message.\n");
	printf("-v --version  Shows the application version.\n");
}

int has_game(int show_error) {
	return file_exists(DEFAULT_FILE, show_error);
}

void backup_game(bool whipe_clean) {
	if (!has_game(false)) {
		return;
	}
	Board *board = Board_read(DEFAULT_FILE);
	Board_save(board, BACKUP_FILE);
	Board_destroy(board);

	copy_file(DEFAULT_MOVES_FILE, BACKUP_MOVES_FILE);
	if (whipe_clean && file_exists(DEFAULT_MOVES_FILE, false)) {
		// Delete old file
		if (remove(DEFAULT_MOVES_FILE) != 0) {
			fprintf(stderr, "Unable to clear moves log!\n");
		}
	}
}

void save_game(Board *board) {
	Board_save(board, DEFAULT_FILE);
}

void save_move(Board *board, Move *move) {
	// Append move
	FILE *file = fopen(DEFAULT_MOVES_FILE, "a");
	if (file == NULL) {
		fprintf(stderr, "Error opening moves file!\n");
		exit(1);
	}
	char *str;
	if (Board_turn(board) == WHITE) {
		str = AN_format(board, move, false, true);
		fprintf(file, "%s", str);
	} else {
		str = AN_format(board, move, false, false);
		fprintf(file, "\t%s\n", str);
	}
	free(str);
	fclose(file);
}

void new_game(int user_color) {
	int human = user_color;
	backup_game(true);
	Board *board = Board_create();
	srand(time(NULL));

	// No prefered color? Coin toss!
	if (user_color == 0) {
		human = (rand() % 100 > 50 ? WHITE : BLACK);
	}

	if (human == WHITE) {
		// User starts. Tell him.
		printf("user plays white\n");
	} else {
		// AI starts, so think of a move:
		Move *move;
		Stats stats = {0, 0, 0};
		move = Engine_turn(board, &stats, Board_turn(board), MAX_PLY_DEPTH, verbosity);
		// Show the move, save it, execute it.
		print_move(board, move);
		save_move(board, move);
		UndoableMove *um = Board_do_move(board, move);
		// Usually we'd do this after a move, but capturing a piece in the first move
		// is probably too challenging for most players
		//Board_add_capture(board, um);
		Undo_destroy(um);
		Move_destroy(move);
	}
	save_game(board);
	Board_destroy(board);
}

void restart_game() {
	if (!has_game(true)) {
		exit(1);
	}
	backup_game(true);
	Board *board = Board_read(DEFAULT_FILE);
	int player = Board_turn(board);
	Board_reset(board);
	if (player == BLACK) {
		// If the AI was white, it gets to start right away
		Move *move;
		Stats stats = {0, 0, 0};
		move = Engine_turn(board, &stats, Board_turn(board), MAX_PLY_DEPTH, verbosity);
		print_move(board, move);
		save_move(board, move);
		UndoableMove *um = Board_do_move(board, move);
		Undo_destroy(um);
		Move_destroy(move);
	} else {
		printf("user plays white\n");
	}
	save_game(board);
	Board_destroy(board);
}

void swap_sides() {
	if (!has_game(true)) {
		exit(1);
		return;
	}
	backup_game(false);
	Board *board = Board_read(DEFAULT_FILE);
	if (board->state != UNFINISHED) {
		fprintf(stderr, "Game has finished.\n");
		Board_destroy(board);
		exit(1);
		return;
	}
	Move *move;
	Stats stats = {0, 0, 0};
	move = Engine_turn(board, &stats, Board_turn(board), MAX_PLY_DEPTH, verbosity);
	print_move(board, move);
	save_move(board, move);
	UndoableMove *um = Board_do_move(board, move);
	Board_add_capture(board, um);
	save_game(board);
	Undo_destroy(um);
	Board_destroy(board);
	Move_destroy(move);
}

void show_board(int simple) {
	if (!has_game(false)) {
		exit(1);
	}
	if (simple) {
		// Simple output the plain file as is
		FILE *file = fopen(DEFAULT_FILE, "r");
		if (file) {
			int c = fgetc(file);
			while (c != EOF) {
				printf ("%c", (char) c);
				c = fgetc(file);
			}
			fclose(file);
		}
	} else {
		// Parse the file and prettyprint the board
		Board *board = Board_read(DEFAULT_FILE);
		Board_print(board, Board_turn(board));
		Board_destroy(board);
	}
}

// Evaluates the current board position, e.g. for generating
// an opening book.
int evaluate() {
	if (!has_game(false)) {
		exit(1);
	}
	Board *board = Board_read(DEFAULT_FILE);
	// Using OPENING_BOOK_MAX_PLY_DEPTH for the search depth here,
	// because this method is meant to be used for generating
	// opening books.
	//Stats stats = {0, 0, 0};
	//Move *move = Engine_turn(board, &stats, Board_turn(board), OPENING_BOOK_MAX_PLY_DEPTH, 0);
	//Move_destroy(move);
	int value = Board_evaluate(board);
	printf("%d\n",value);
	Board_destroy(board);
	return value;
}

char * print_move(Board *board, Move *move) {
	char *str;
	if (algebraic) {
		str = AN_format(board, move, true, true);
	} else {
		str = Simple_move_format(board, move, true);
	}
	printf("%s\n", str);
	return str;
}

Move * parse_move(char *str, Board *board) {
	if (algebraic) {
		return AN_parse(str, board);
	} else {
		return Simple_move_parse(str, board);
	}
}

void show_moves() {
	if (!has_game(false)) {
		exit(1);
	}
	Board *board = Board_read(DEFAULT_FILE);
	// Generate all moves:
	Move *head = Move_alloc();
	int color = Board_turn(board);
	int total = v_get_all_valid_moves_for_color(&head, board, color);
	if (total == 0) {
		printf("No available moves.");
		return;
	}
	Move *curr = head;
	int i = 1;
	while (curr) {
		char *move;
		if (algebraic) {
			move = AN_format(board, curr, false, false);
			printf("%d. %s\n", i, move);
		} else {
			move = Simple_move_format(board, curr, true);
			printf("%s\n", move);
		}
		free(move);
		curr = curr->next_sibling;
		i++;
	}
	Move_destroy(head);
}

int backup_to_slot(int slot) {
	if (slot == -1) {
		return 1;
	}
	if (!has_game(true)) {
		return 1;
	}
	Board *board = Board_read(DEFAULT_FILE);
	char *file = malloc((strlen(SLOT_FILE) + 1) * sizeof(char));
	sprintf(file, SLOT_FILE, slot);
	Board_save(board, file);
	Board_destroy(board);

	char *file2 = malloc((strlen(SLOT_MOVES_FILE) + 1) * sizeof(char));
	sprintf(file2, SLOT_MOVES_FILE, slot);
	copy_file(DEFAULT_MOVES_FILE, file2);
	return 0;
}

int restore_from_slot(int slot) {
	if (slot == -1) {
		return 1;
	}
	char *file = malloc((strlen(SLOT_FILE) + 1) * sizeof(char));
	sprintf(file, SLOT_FILE, slot);
	Board *board = Board_read(file);
	Board_save(board, DEFAULT_FILE);
	Board_print(board, Board_turn(board));
	Board_destroy(board);

	char *file2 = malloc((strlen(SLOT_MOVES_FILE) + 1) * sizeof(char));
	sprintf(file2, SLOT_MOVES_FILE, slot);
	copy_file(file2, DEFAULT_MOVES_FILE);
	return 0;
}

int get_game_slot(char *arg) {
	char * endptr;
	int slot = strtol(arg, &endptr, 0);
	if (endptr == arg) {
		fprintf(stderr, "Unable to parse game slot number %s.\n", arg);
		return -1;
	} else {
		if (slot > 9 || slot < 0) {
			fprintf(stderr, "Game slot not available; only values in the range 0-9 are allowed.\n");
			return -1;
		}
		return slot;
	}
}

void prepare_filenames() {
	DEFAULT_FILE = with_user_dir(DEFAULT_FILE);
	SLOT_FILE = with_user_dir(SLOT_FILE);
	SLOT_MOVES_FILE = with_user_dir(SLOT_MOVES_FILE);
	BACKUP_FILE = with_user_dir(BACKUP_FILE);
	DEFAULT_MOVES_FILE = with_user_dir(DEFAULT_MOVES_FILE);
	BACKUP_MOVES_FILE = with_user_dir(BACKUP_MOVES_FILE);
}
