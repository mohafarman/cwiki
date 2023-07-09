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
    // cwiki_user_data = malloc(sizeof(cwiki_user_s));
    cwiki_user_data = cwiki_init_user_s(cwiki_user_data);

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

        /* TODO: Curl article */
        if (cwiki_curl_article(cwiki_user_data) == -1) {
            break;
        }

        /* NOTE: consider using panel.h for viewing article and toc, so user can also cycle between them */
        cwiki_tui_window_article_view(cwiki_user_data);

        /* used to continue the program and displays the search bar */
        // ungetch(' ');

    } while ( (c = wgetch(stdscr)) != 'q' );

    endwin();

    printf("URL: %s\n", cwiki_user_data->url);
    /* Prints gibberish, somethings wrong with memory alloc */
    printf("size: %lu\n", cwiki_user_data->url_response_size);
    printf("RESPONSE: %s\n", cwiki_user_data->url_response);

    free(cwiki_user_data);

    zlog_info(log_debug, "Terminating application");
    return 0;
}

cwiki_user_s *cwiki_init_user_s(cwiki_user_s* cwiki_user_data) {
    cwiki_user_data = calloc(1, sizeof(cwiki_user_s));

    strcpy(cwiki_user_data->text_search, "");
    cwiki_user_data->url = NULL;
    cwiki_user_data->url_response = NULL;
    cwiki_user_data->url_response_size = 0;
    cwiki_user_data->url_response_parsed[9][4] = calloc(1, sizeof cwiki_user_data->url_response_parsed[9][4]);
    cwiki_user_data->selected_article_title = NULL;
    cwiki_user_data->selected_article_pageid = 0;
    cwiki_user_data->url_article = NULL;
    cwiki_user_data->url_article_response = NULL;
    cwiki_user_data->url_article_response_size = 0;

    return cwiki_user_data;
}
