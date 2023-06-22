#include "../include/cwiki.h"
#include "../include/cwiki_tui.h"
#include "../include/cwiki_log.h"
#include "../include/cwiki_curl.h"
#include "../include/cwiki_utils.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

cwiki_user *cwiki_user_data;

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    char c;

    // parse command line options
    options_s opts = {0};

    cwiki_utils_parse_args(argc, argv, &opts);

    if (opts.version) {
        cwiki_utils_version(stdout);
        return EXIT_SUCCESS;
    }

    if (opts.help) {
        cwiki_utils_help(stdout);
        return EXIT_SUCCESS;
    }

    /* Initialize user data */
    /* NOTE: text_search only reads in a word, not sentences */
    cwiki_user_data = malloc(sizeof(cwiki_user));

    // 1. Logging
    cwiki_log_init_debug();

    /* Setup ncurses */
    cwiki_tui_init_ncurses();

    do  {
        cwiki_tui_window_search();
    } while ( (c = wgetch(stdscr)) != 'q' );

    endwin();
    zlog_info(log_debug, "Terminating application");

    printf("cwiki_user_data->text_search: %s\n", cwiki_user_data->text_search);
    return 0;
}
