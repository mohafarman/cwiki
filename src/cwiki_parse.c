#include "../include/cwiki_parse.h"

#include <cjson/cJSON.h>

int cwiki_parse_search(cwiki_user_s* cwiki_user_data) {
    int status = 0;
    int row = 0, col = 0;
    const cJSON *query = NULL;
    const cJSON *search = NULL;
    const cJSON *titles = NULL;
    char *string_title = NULL;
    char *string_pageid = NULL;
    char *string_snippet = NULL;

    cJSON *search_json = cJSON_Parse(cwiki_user_data->url_response);

    if (search_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = -1;
        goto end;
    }

    query = cJSON_GetObjectItemCaseSensitive(search_json, "query");
    search = cJSON_GetObjectItemCaseSensitive(query, "search");

    cJSON_ArrayForEach(titles, search)
    {
        cJSON *title = cJSON_GetObjectItemCaseSensitive(titles, "title");
        cJSON *pageid = cJSON_GetObjectItemCaseSensitive(titles, "pageid");
        cJSON *snippet = cJSON_GetObjectItemCaseSensitive(titles, "snippet");

        if (!cJSON_IsString(title) || !cJSON_IsNumber(pageid)) {
            status = -1;
            goto end;
        }
        string_title = cJSON_Print(title);
        string_pageid = cJSON_Print(pageid);
        string_snippet = cJSON_Print(snippet);

        cwiki_user_data->url_response_parsed[row][col] = string_title;
        cwiki_user_data->url_response_parsed[row][col+1] = string_pageid;
        cwiki_user_data->url_response_parsed[row][col+2] = string_snippet;
        ++row;
    }

    /* Successfully parsed */
    status = 0;
    goto end;

end:
    cJSON_Delete(search_json);
    return status;
}
