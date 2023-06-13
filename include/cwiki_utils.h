#ifndef CWIKI_UTILS_H_
#define CWIKI_UTILS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct options_s {
  bool help; // show version and exit
  bool version; // show version and exit
} options_s;

/*
 * Parse command line args into the provided options_s struct.
 */
void cwiki_utils_parse_args(int argc, char **argv, options_s *opts);

/*
 * Print version information.
 */
void cwiki_utils_version(FILE *where);

/*
 * Print help information.
 */
void cwiki_utils_help(FILE *where);

#endif // CWIKI_UTILS_H_