#include "../include/cwiki_curl.h"

#include <stdlib.h>
#include <string.h>

#define URL_BUFSIZE 128

int cwiki_curl_set_url_search() {
  char wiki_url[URL_BUFSIZE] = "https://en.wikipedia.org/w/api.php?action=query&list=search&srsearch=";
  const char *wiki_url_format = "&format=json";

  /* create the url */
  strcat(wiki_url, cwiki_user_data->search);
  /* specify format response from wikipedia, json here */
  strcat(wiki_url, wiki_url_format);

  cwiki_user_data->url = malloc(URL_BUFSIZE);
  strcpy(cwiki_user_data->url, wiki_url);

  return 0;
}

int cwiki_curl_set_url_article() {
  char wiki_url[URL_BUFSIZE] = "http://en.wikipedia.org/w/api.php?action=query&format=json&prop=extracts&titles=";
  const char *wiki_url_content = "&formatversion=2";

  /* Create the url */
  strcat(wiki_url, cwiki_user_data->selected_article_title);
  strcat(wiki_url, wiki_url_content);

  /* Copy it over to our struct */
  char *ptr = realloc(cwiki_user_data->url, strlen(wiki_url) + 1);
  if(!ptr) {
    /* out of response! */
    printf("not enough response (realloc returned NULL)\n");
    return 0;
  }

  cwiki_user_data->url = ptr;
  strcpy(cwiki_user_data->url, wiki_url);

  return 0;
}

size_t cwiki_curl_callback_article(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  cwiki_user_s *mem = (cwiki_user_s *)userp;

  char *ptr = realloc(mem->url_response_article, mem->url_response_article_size + realsize + 1);
  if(!ptr) {
    /* out of response! */
    printf("not enough response (realloc returned NULL)\n");
    return 0;
  }

  mem->url_response_article = ptr;
  memcpy(&(mem->url_response_article[mem->url_response_article_size]), contents, realsize);
  mem->url_response_article_size += realsize;
  mem->url_response_article[mem->url_response_article_size] = 0;

  return realsize;
}

size_t cwiki_curl_callback_search(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  cwiki_user_s *mem = (cwiki_user_s *)userp;

  char *ptr = realloc(mem->url_response_search, mem->url_response_search_size + realsize + 1);
  if(!ptr) {
    /* out of response! */
    printf("not enough response (realloc returned NULL)\n");
    return 0;
  }

  mem->url_response_search = ptr;
  memcpy(&(mem->url_response_search[mem->url_response_search_size]), contents, realsize);
  mem->url_response_search_size += realsize;
  mem->url_response_search[mem->url_response_search_size] = 0;

  return realsize;
}

int cwiki_curl_url(enum cwiki_curl curl) {
  CURL *curl_handle;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

  switch(curl) {
    case SEARCH:
      cwiki_curl_set_url_search();
      /* specify URL to get */
      curl_easy_setopt(curl_handle, CURLOPT_URL, cwiki_user_data->url);
      curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, cwiki_curl_callback_search);
      break;

    case ARTICLE:
      cwiki_curl_set_url_article();
      /* specify URL to get */
      curl_easy_setopt(curl_handle, CURLOPT_URL, cwiki_user_data->url);
      curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, cwiki_curl_callback_article);
      break;

    default:
      return -1;
  }

  /* we pass our 'cwiki_user_data-> struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)cwiki_user_data);

  /* some servers do not like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  /* get it! */
  res = curl_easy_perform(curl_handle);

  /* check for errors */
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
    return -1;
  }
  else {
    /*
     * Now, our cwiki_user_data->memory points to a memory block that is cwiki_user_data->size
     * bytes big and contains the remote file.
     *
     * Do something nice with it!
     */
  }

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);

  /* we are done with libcurl, so clean it up */
  curl_global_cleanup();
  return 0;
}

int cwiki_curl_url_debug() {
  CURL *curl;
  CURLcode res;
  struct data config;

  config.trace_ascii = 1; /* enable ascii tracing */

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &config);

    /* the DEBUGFUNCTION has no effect until we enable VERBOSE */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    /* example.com is redirected, so we tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(curl, CURLOPT_URL, cwiki_user_data->url);
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return 0;
}

int my_trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp)
{
  struct data *config = (struct data *)userp;
  const char *text;
  (void)handle; /* prevent compiler warning */

  switch(type) {
  case CURLINFO_TEXT:
    fprintf(stderr, "== Info: %s", data);
    /* FALLTHROUGH */
  default: /* in case a new one is introduced to shock us */
    return 0;

  case CURLINFO_HEADER_OUT:
    text = "=> Send header";
    break;
  case CURLINFO_DATA_OUT:
    text = "=> Send data";
    break;
  case CURLINFO_SSL_DATA_OUT:
    text = "=> Send SSL data";
    break;
  case CURLINFO_HEADER_IN:
    text = "<= Recv header";
    break;
  case CURLINFO_DATA_IN:
    text = "<= Recv data";
    break;
  case CURLINFO_SSL_DATA_IN:
    text = "<= Recv SSL data";
    break;
  }

  dump(text, stderr, (unsigned char *)data, size, config->trace_ascii);
  return 0;
}

void dump(const char *text, FILE *stream, unsigned char *ptr, size_t size, char nohex)
{
  size_t i;
  size_t c;

  unsigned int width = 0x10;

  if(nohex)
    /* without the hex output, we can fit more on screen */
    width = 0x40;

  fprintf(stream, "%s, %10.10lu bytes (0x%8.8lx)\n",
          text, (unsigned long)size, (unsigned long)size);

  for(i = 0; i<size; i += width) {

    fprintf(stream, "%4.4lx: ", (unsigned long)i);

    if(!nohex) {
      /* hex not disabled, show it */
      for(c = 0; c < width; c++)
        if(i + c < size)
          fprintf(stream, "%02x ", ptr[i + c]);
        else
          fputs("   ", stream);
    }

    for(c = 0; (c < width) && (i + c < size); c++) {
      /* check for 0D0A; if found, skip past and start a new line of output */
      if(nohex && (i + c + 1 < size) && ptr[i + c] == 0x0D &&
         ptr[i + c + 1] == 0x0A) {
        i += (c + 2 - width);
        break;
      }
      fprintf(stream, "%c",
              (ptr[i + c] >= 0x20) && (ptr[i + c]<0x80)?ptr[i + c]:'.');
      /* check again for 0D0A, to avoid an extra \n if it's at width */
      if(nohex && (i + c + 2 < size) && ptr[i + c + 1] == 0x0D &&
         ptr[i + c + 2] == 0x0A) {
        i += (c + 3 - width);
        break;
      }
    }
    fputc('\n', stream); /* newline */
  }
  fflush(stream);
}
