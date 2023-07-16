#include "../include/cwiki.h"
#include "../include/cwiki_tui.h"
#include "../include/cwiki_curl.h"
#include "../include/cwiki_utils.h"
#include "../include/cwiki_parse.h"

#include <unistd.h>
#include <stdlib.h>

cwiki_user_s *cwiki_user_data;
cwiki_article_s *cwiki_article_data;

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

    /* Initialize program defined data */
    cwiki_init_user();
    cwiki_init_article();

    enum cwiki_curl curl;

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
        /* NOTE: Why not just do the parsing in the curl file? */
        /* NOTE: I can follow the examples from the curl website (htmltidy.c) */
        /*       to also manage the html */
        /* NOTE: Maybe not. Might be easier to manage everything if the html
         *       is parsed on its own */
        if (cwiki_curl_url(curl) != -1) {
            // printf("%lu bytes retrieved\n", (unsigned long)cwiki_user_data->url_response_article_size);
            // printf("%s\n", cwiki_user_data->url_response_article);
        }
        // cwiki_curl_url_debug();

        cwiki_parse_article();

        /* NOTE: returns -1 */
        if (cwiki_parse_html_article() == -1) {
            break;
        }

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
    // printf("Article parsed: %s\n", cwiki_user_data->url_response_article_parsed);
    // printf("A Title: %s\n", cwiki_article_data->headers[1]);
    printf("A Title: %d\n", cwiki_article_data->headers_count);

    cwiki_destroy_user();
    cwiki_destroy_article();

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

void cwiki_init_article() {
    cwiki_article_data = calloc(1, sizeof(cwiki_article_s));

    cwiki_article_data->paragraph = NULL;
    cwiki_article_data->headers = NULL;
    cwiki_article_data->headers_count = 0;
}

void cwiki_destroy_user() {
    free(cwiki_user_data->search);
    free(cwiki_user_data->url_response_search);
    free(cwiki_user_data->selected_article_title);
    free(cwiki_user_data->url_response_article);
    free(cwiki_user_data);
}

void cwiki_destroy_article() {
    if (cwiki_article_data->paragraph != NULL) {
        free(cwiki_article_data->paragraph);
    }
    if (cwiki_article_data->headers != NULL) {
        free(cwiki_article_data->headers);
    }
}
