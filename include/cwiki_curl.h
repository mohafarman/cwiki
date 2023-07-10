#ifndef CWIKI_CURL_H_
#define CWIKI_CURL_H_

#include <stdio.h>
#include <stdbool.h>

#include "cwiki.h"

typedef struct {
    char *response;
    size_t response_size;
} MemoryStruct;

int cwiki_curl_url(cwiki_user_s*, bool);
int cwiki_curl_article(cwiki_user_s*);

int cwiki_curl_url_search(cwiki_user_s*);
int cwiki_curl_url_article(cwiki_user_s*);

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

#endif // CWIKI_CURL_H_
