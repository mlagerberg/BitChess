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
 * Returns true if the given file exists.
 * Show errors to the user when things go wrong
 * and show_error is enabled.
 */
bool file_exists(const char *file, bool show_error);

/**
 * Copies a file, returns success
 */
bool copy_file(const char *old_filename, const char *new_filename);

#endif