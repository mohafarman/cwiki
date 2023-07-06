#include "../include/cwiki.h"
#include "../include/cwiki_tui.h"
#include "../include/cwiki_log.h"
#include "../include/cwiki_curl.h"
#include "../include/cwiki_utils.h"
#include "../include/cwiki_parse.h"

#include <unistd.h>
#include <stdlib.h>

cwiki_user_s *cwiki_user_data;

int main(int argc, char *argv[]) {
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
    cwiki_user_data = malloc(sizeof(cwiki_user_s));

    // 1. Logging
    cwiki_log_init_debug();

    /* Setup ncurses */
    cwiki_tui_init_ncurses();

     do
     {
        /* Print logo */
        cwiki_tui_screen_logo();
        refresh();
        /* User search article */
        cwiki_tui_window_search();

        cwiki_tui_screen_clear();

        /* curl user search */
        if (cwiki_curl_url(cwiki_user_data) != -1) {
            // printf("%lu bytes retrieved\n", (unsigned long)cwiki_user_data->url_response_size);
            // printf("%s\n", cwiki_user_data->url_response);
        }

        /* parse wikipedia response */
        if (cwiki_parse_search(cwiki_user_data) == -1) {
            printf("failed parsing\n");
            break;
        }

        /* show results */
        /* NOTE: User can now select an article */
        cwiki_tui_window_articles(cwiki_user_data);

        /* NOTE: consider using panel.h for viewing article and toc, so user can also cycle between them */

        /* used to continue the program and displays the search bar */
        ungetch(' ');

    } while ( (c = wgetch(stdscr)) != 'q' );

    endwin();

    free(cwiki_user_data);

    zlog_info(log_debug, "Terminating application");
    return 0;
}
