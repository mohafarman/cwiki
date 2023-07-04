#include "../include/cwiki_utils.h"

#include <getopt.h> // opterr, optarg, etc.
#include <unistd.h> // getopt(), STDOUT_FILENO
#include <stdbool.h>
#include <string.h> // strlen

#define PROGRAM_NAME "cwiki"
#define PROGRAM_URL "https://github.com/mohafarman/cwiki"
#define PROGRAM_AUTHOR "Mohamad Farman"
#define PROGRAM_VER_MAJOR 0
#define PROGRAM_VER_MINOR 1
#define PROGRAM_VER_PATCH 1

void cwiki_utils_parse_args(int argc, char **argv, options_s *opts)
{
    opterr = 0;
    int o;
    while ((o = getopt(argc, argv, ":vh")) != -1) {
        switch (o) {
            case 'h':
                opts->help = true;
                break;
            case 'v':
                opts->version = true;
                break;
            case ':': /* -p without operand */
                fprintf(stderr, "Option -%c requires an operand\n", optopt);
                break;
            case '?':
                fprintf(stderr, "Unrecognized option: '-%c'\n", optopt);
        }
    }
}

void cwiki_utils_version(FILE *where)
{
    fprintf(where, "\t%s %d.%d.%d\nURL:\t%s\nAuthor:\t%s\n", PROGRAM_NAME, PROGRAM_VER_MAJOR,
            PROGRAM_VER_MINOR, PROGRAM_VER_PATCH, PROGRAM_URL, PROGRAM_AUTHOR);
}

void cwiki_utils_help(FILE *where)
{
    fprintf(where, "USAGE\n");
    fprintf(where, "\t%s [OPTIONS...]\n", PROGRAM_NAME);
    fprintf(where, "OPTIONS\n");
    fprintf(where, "\t-h\tprint this help text and exit\n");
    fprintf(where, "\t-v\tprint version information and exit\n");
}

void cwiki_utils_remove_html_tags(char *str) {
    int inside_tag = 0;
    char *src = str;
    char *dest = str;

    while (*src) {
        if (*src == '<') {
            inside_tag = 1;
            src++;
            continue;
        }
        if (*src == '>') {
            inside_tag = 0;
            src++;
            continue;
        }
        if (!inside_tag) {
            *dest = *src;
            dest++;
        }
        src++;
    }

    *dest = '\0';
}

void cwiki_utils_remove_quotation_marks(char *str) {
    int len = strlen(str);

    if (str[0] == '"' && str[len - 1] == '"') {
        strcpy(str, str + 1);
        str[len - 2] = '\0';
    }
}

void cwiki_utils_prettify(char *str) {
    cwiki_utils_remove_html_tags(str);
    cwiki_utils_remove_quotation_marks(str);
}
