
/**
 * color.h
 * 
 * Adds some color codes that can be used for colored output
 * to the console.
 *
 * Usage: 
 * 	printf("%sHello %sworld!%s", red, blue, none);
 */
#ifndef _COLOR_H_
#define _COLOR_H_

#define color_white "\033[1;37m"
#define color_black "\033[1;30m"
#define TILE_WHITE "\033[0;37;47m"
#define TILE_BLACK "\033[0;36;46m"

#define red   "\033[0;31m"
#define cyan  "\033[1;36m"
#define green "\033[4;32m"
#define blue  "\033[9;34m"
#define black  "\033[0;30m"
#define brown  "\033[0;33m"
#define magenta  "\033[0;35m"
#define gray  "\033[0;37m"

#define resetcolor   "\033[0m"

#endif