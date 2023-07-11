#ifndef CWIKI_CURL_H_
#define CWIKI_CURL_H_

#include "cwiki.h"

#include <stdio.h>
#include <stdbool.h>
#include <curl/curl.h>

enum cwiki_curl { SEARCH, ARTICLE };

typedef struct {
    char *response;
    size_t response_size;
} MemoryStruct;

struct data {
  char trace_ascii; /* 1 or 0 */
};

int cwiki_curl_url(enum cwiki_curl);
int cwiki_curl_url_debug();

int cwiki_curl_set_url_search();
int cwiki_curl_set_url_article();

size_t cwiki_curl_callback_article(void *, size_t, size_t, void *);
size_t cwiki_curl_callback_search(void *, size_t, size_t, void *);


int my_trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp);
void dump(const char *text, FILE *stream, unsigned char *ptr, size_t size, char nohex);

#endif // CWIKI_CURL_H_
