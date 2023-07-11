#ifndef CWIKI_H_
#define CWIKI_H_

#include <stdio.h>

typedef struct {
    char *search;
    char *url;
    char *url_response_search;
    size_t url_response_search_size;
    char *url_response_parsed[10][5]; /* title, pageid, snippet, wordcount, timestamp */
    char *selected_article_title;
    int selected_article_pageid;
    char *url_response_article;
    size_t url_response_article_size;
} cwiki_user_s;

extern cwiki_user_s *cwiki_user_data;

cwiki_user_s *cwiki_init_user_s(cwiki_user_s*);

/*
 * Create an enum with different states of the program
 * E.g:
 * 1. Searching
 * 2. Viewing text
 * 3. Viewing TOC
 * 4.  */

#endif // CWIKI_H_
