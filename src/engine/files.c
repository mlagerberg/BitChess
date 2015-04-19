#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "files.h"
#if defined(_WIN32)
#include <windows.h>	// _mkdir
#else
#include <sys/stat.h>	// mkdir
#include <sys/types.h>	// S_IRWXO
#endif

// Holder for the user dir
static char *HOME_DIR = NULL;

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

#if defined(_WIN32) && defined(__GNUC__)
// user_dir for Windows
char* user_dir() {
	if (HOME_DIR == NULL) {
		if (getenv("USERPROFILE") == NULL) {
			char *homedrive, *homepath;
			homedrive = strdup(getenv("HOMEDRIVE"));
			homepath  = strdup(getenv("HOMEPATH"));
			HOME_DIR = (char*) malloc((strlen(homedrive) + strlen(homepath) + strlen(STORAGE_DIR)) * sizeof(char));
			strcpy(HOME_DIR, homedrive);
			strcat(HOME_DIR, homepath);
			strcat(HOME_DIR, STORAGE_DIR);
		} else {
			char *homepath;
			homepath = strdup(getenv("USERPROFILE"));
			HOME_DIR = (char*) malloc((strlen(homepath) + strlen(STORAGE_DIR)) * sizeof(char));
			strcpy(HOME_DIR, homepath);
			strcat(HOME_DIR, STORAGE_DIR);
		}
		CreateDirectory(HOME_DIR, NULL);
	}
	return HOME_DIR;
}

#else 

// user_dir for Linux and Mac
char* user_dir() {
	if (HOME_DIR == NULL) {
		char *temp = strdup(getenv("HOME"));
		if (temp == NULL) {
			HOME_DIR = (char*) malloc((2 + strlen(STORAGE_DIR)) * sizeof(char));
			strcpy(HOME_DIR, "~");
		} else {
			HOME_DIR = (char*) malloc((strlen(temp) + strlen(STORAGE_DIR)) * sizeof(char));
			strcpy(HOME_DIR, temp);
		}
		strcat(HOME_DIR, STORAGE_DIR);
		mkdir(HOME_DIR, S_IRWXO);
	}
	return HOME_DIR;
}

#endif

char* with_user_dir(char* filename) {
	char* result;
	char* dir = user_dir();
	result = (char*) malloc((strlen(dir) + strlen(STORAGE_DIR)) * sizeof(char));
	strcpy(result, dir);
	strcat(result, filename);
	return result;
}