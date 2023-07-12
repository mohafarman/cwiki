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
    cwiki_init_user();

    enum cwiki_curl curl;

    // 1. Logging
    cwiki_log_init_debug();

    /* Setup ncurses */
    cwiki_tui_init_ncurses();

     do
     {
        /* refresh is called twice from here */
        cwiki_tui_screen_clear();
        /* Print logo */
        cwiki_tui_screen_logo();
        refresh();
        /* to here, unnecessary */

        /* User search article */
        cwiki_tui_window_search();

        cwiki_tui_screen_clear();

        curl = SEARCH;
        /* curl user search */
        if (cwiki_curl_url(curl) != -1) {
            // printf("%lu bytes retrieved\n", (unsigned long)cwiki_user_data->url_response_size);
            // printf("%s\n", cwiki_user_data->url_response);
        }

        /* parse wikipedia response */
        if (cwiki_parse_search() == -1) {
            printf("failed parsing\n");
            break;
        }

        /* show results */
        /* NOTE: User can now select an article */
        cwiki_tui_window_articles();

        curl = ARTICLE;
        if (cwiki_curl_url(curl) != -1) {
            // printf("%lu bytes retrieved\n", (unsigned long)cwiki_user_data->url_response_article_size);
            // printf("%s\n", cwiki_user_data->url_response_article);
        }
        // cwiki_curl_url_debug();

        cwiki_parse_article();

        /* NOTE: consider using panel.h for viewing article and toc, so user can also cycle between them */
        cwiki_tui_window_article_view();

        /* used to continue the program and displays the search bar */
        // ungetch(' ');

    } while ( (c = wgetch(stdscr)) != 'q' );

    endwin();

    /* Debugging purposes */
    printf("URL: %s\n", cwiki_user_data->url);
    printf("size: %lu\n", cwiki_user_data->url_response_article_size);
    // printf("Response: %s\n", cwiki_user_data->url_response_search);
    // printf("Response: %s\n", cwiki_user_data->url_response_article);
    printf("Article parsed: %s\n", cwiki_user_data->url_response_article_parsed);

    cwiki_destroy_user();

    zlog_info(log_debug, "Terminating application");
    return 0;
}

void cwiki_init_user() {
    cwiki_user_data = calloc(1, sizeof(cwiki_user_s));

    cwiki_user_data->search = NULL;
    cwiki_user_data->url = NULL;
    cwiki_user_data->url_response_search = malloc(1);
    cwiki_user_data->url_response_search_size = 0;
    cwiki_user_data->url_response_parsed[9][4] = calloc(1, sizeof cwiki_user_data->url_response_parsed[9][4]);
    cwiki_user_data->selected_article_title = NULL;
    cwiki_user_data->selected_article_pageid = 0;
    cwiki_user_data->url_response_article = malloc(1);
    cwiki_user_data->url_response_article_size = 0;
    cwiki_user_data->url_response_article_parsed = NULL;
}

void cwiki_destroy_user() {
    free(cwiki_user_data->search);
    free(cwiki_user_data->url_response_search);
    free(cwiki_user_data->selected_article_title);
    free(cwiki_user_data->url_response_article);
    free(cwiki_user_data);
}
