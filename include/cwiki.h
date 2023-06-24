#ifndef CWIKI_H_
#define CWIKI_H_

#include <stdio.h>

typedef enum { search, view_article, view_toc } cwiki_state;

typedef struct {
    char text_search[52];
} cwiki_user;

extern cwiki_user *cwiki_user_data;

/*
 * Create an enum with different states of the program
 * E.g:
 * 1. Searching
 * 2. Viewing text
 * 3. Viewing TOC
 * 4.  */

#endif // CWIKI_H_
