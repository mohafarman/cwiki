#include "../include/cwiki_curl.h"

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

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

int cwiki_curl_url(cwiki_user_s *cwiki_user_data) {
  CURL *curl_handle;
  CURLcode res;

  // MemoryStruct chunk;

  // cwiki_user_data->response = malloc(1);  /* will be grown as needed by the realloc above */
  cwiki_user_data->url_response_size = 0;    /* no data at this point */

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  /* specify URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_URL, cwiki_user_data->text_search);

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

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

    // printf("%lu bytes retrieved\n", (unsigned long)cwiki_user_data->url_response_size);
    // printf("%s\n", cwiki_user_data->url_response);
  }

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);

  // free(cwiki_user_data->url_response);

  /* we are done with libcurl, so clean it up */
  curl_global_cleanup();
  return 0;
}
