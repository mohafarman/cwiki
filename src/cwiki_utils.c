#include "../include/cwiki_utils.h"

#include <getopt.h> // opterr, optarg, etc.
#include <unistd.h> // getopt(), STDOUT_FILENO
#include <stdbool.h>

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
