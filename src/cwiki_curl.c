#include "../include/cwiki_curl.h"

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define URL_BUFSIZE 128

int cwiki_curl_url_search(cwiki_user_s* cwiki_user_data) {
  char wiki_url[URL_BUFSIZE] = "https://en.wikipedia.org/w/api.php?action=query&list=search&srsearch=";
  const char *wiki_url_format = "&format=json";

  /* create the url */
  strcat(wiki_url, cwiki_user_data->text_search);
  /* specify format response from wikipedia, json here */
  strcat(wiki_url, wiki_url_format);

  cwiki_user_data->url = malloc(URL_BUFSIZE);
  strcpy(cwiki_user_data->url, wiki_url);

  return 0;
}

int cwiki_curl_url_article(cwiki_user_s* cwiki_user_data) {
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

size_t WriteMemoryCallback2(void *contents, size_t size, size_t nmemb, void *userp)
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

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  cwiki_user_s *mem = (cwiki_user_s *)userp;

  char *ptr = realloc(mem->url_response, mem->url_response_size + realsize + 1);
  if(!ptr) {
    /* out of response! */
    printf("not enough response (realloc returned NULL)\n");
    return 0;
  }

  mem->url_response = ptr;
  memcpy(&(mem->url_response[mem->url_response_size]), contents, realsize);
  mem->url_response_size += realsize;
  mem->url_response[mem->url_response_size] = 0;

  return realsize;
}

int cwiki_curl_url(cwiki_user_s *cwiki_user_data, bool flag) {
  CURL *curl_handle;
  CURLcode res;

  cwiki_user_data->url_response_size = 0;    /* no data at this point */

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  /* specify URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_URL, cwiki_user_data->url);

   if (flag) {
     /* send all data to this function  */
     curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback2);
   }
   else {
     /* send all data to this function  */
     curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
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
