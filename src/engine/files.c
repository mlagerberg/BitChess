#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "common.h"
#include "files.h"


bool file_exists(const char *filename, bool show_error) {
	// Check if file exists:
	FILE *file;
	if ((file = fopen(filename, "r")) == NULL) {
		if (errno == ENOENT) {
			if (show_error) {
				fprintf(stderr, "No game file found. Please use 'chess new' to start a new game.\n");
			}
			return false;
		} else {
			// Check for other errors too, like EACCES and EISDIR
			if (show_error) {
				fprintf(stderr, "Some error occured while reading game file.\n");
			}
			fclose(file);
			return false;
		}
	} else {
		fclose(file);
	}
	return true;
}

// From a comment found here:
// http://www.codingunit.com/c-tutorial-copying-a-file-in-c
bool copy_file(const char *from, const char *to) {
	FILE *ffrom = fopen(from, "r");
	if (!ffrom) {
		return false;
	}
	FILE *fto = fopen(to, "w");
	if (!fto) {
		fclose(fto);
		return false;
	}
	int inch;
	while ((inch = fgetc(ffrom))) {
		if (inch == EOF
				|| fputc(inch, fto) == EOF) {
			break;
		}
	}
	fclose(fto);
	fclose(ffrom);
	return true;
}
