#include <stdbool.h>
#include "common.h"

/**
 * fitness.h / fitness.c
 *
 * Simple file I/O helper functions
 *
 */

#ifndef _FILES_H_
#define _FILES_H_

/**
 * Storage path of the game files, relative to the user's home folder
 */
#define STORAGE_DIR "/.BitChess/"

/**
 * Returns true if the given file exists.
 * Show errors to the user when things go wrong
 * and show_error is enabled.
 */
bool file_exists(const char *file, bool show_error);

/**
 * Copies a file, returns success
 */
bool copy_file(const char *old_filename, const char *new_filename);

/**
 * Returns the user's home directory path
 */
 char* user_dir();

 /**
  * Prepends the filename with the user's home dir path
  * and STORAGE_DIR
  */
 char* with_user_dir(char* filename);

#endif