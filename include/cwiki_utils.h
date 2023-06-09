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

/*
 * Removes html tags. Generated with help of ChatGPT.
 */
void cwiki_utils_remove_html_tags(char *);

/*
 * Removes quotations marks. Generated with help of ChatGPT.
 */
void cwiki_utils_remove_quotation_marks(char *);

/*
 * Encode spaces with %20, required for HTTP request
 */
char *cwiki_utils_encode_spaces(char *);

/* Combines remove_html_tags and remove_quotation_marks */
void cwiki_utils_prettify(char *);

/*
 * Removes the time at the end and preserved the date
*/
void cwiki_utils_get_date(char *str);

/*
 * Return number of pages based on word count
 */
char *cwiki_utils_calculate_pages(char *str_words);

#endif // CWIKI_UTILS_H_
