#include "../include/cwiki_parse.h"
#include "../include/cwiki_utils.h"

#include <cjson/cJSON.h>
#include <stdlib.h>
#include <string.h>

int cwiki_parse_search() {
    int status = 0;
    int row = 0;
    int col_title = 0, col_pageid = 1, col_snippet = 2;
    int col_pages = 3, col_timestamp = 4;

    const cJSON *query = NULL;
    const cJSON *search = NULL;
    const cJSON *titles = NULL;
    char *string_title = NULL;
    char *string_pageid = NULL;
    char *string_snippet = NULL;
    char *string_wordcount = NULL;
    char *string_pages = NULL;
    char *string_timestamp = NULL;
    // char *string_snippet_pretty = NULL;

    cJSON *search_json = cJSON_Parse(cwiki_user_data->url_response_search);

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
        cJSON *wordcount = cJSON_GetObjectItemCaseSensitive(titles, "wordcount");
        cJSON *timestamp = cJSON_GetObjectItemCaseSensitive(titles, "timestamp");

        if (!cJSON_IsString(title) || !cJSON_IsNumber(pageid) || !cJSON_IsString(snippet) ||
            !cJSON_IsNumber(wordcount) || !cJSON_IsString(timestamp)) {
            status = -1;
            goto end;
        }
        string_title = cJSON_Print(title);
        string_pageid = cJSON_Print(pageid);
        string_snippet = cJSON_Print(snippet);
        string_wordcount = cJSON_Print(wordcount);
        string_timestamp = cJSON_Print(timestamp);

        /* Prettify the strings */
        cwiki_utils_remove_quotation_marks(string_title);
        cwiki_utils_remove_quotation_marks(string_pageid);
        cwiki_utils_remove_quotation_marks(string_wordcount);
        cwiki_utils_remove_quotation_marks(string_timestamp);

        cwiki_utils_get_date(string_timestamp);

        cwiki_utils_remove_html_tags(string_snippet);
        cwiki_utils_remove_quotation_marks(string_snippet);

        string_pages = cwiki_utils_calculate_pages(string_wordcount);

        cwiki_user_data->url_response_parsed[row][col_title] = string_title;          /* [][0] */
        cwiki_user_data->url_response_parsed[row][col_pageid] = string_pageid;        /* [][1] */
        cwiki_user_data->url_response_parsed[row][col_snippet] = string_snippet;      /* [][2] */
        cwiki_user_data->url_response_parsed[row][col_pages] = string_pages;          /* [][3] */
        cwiki_user_data->url_response_parsed[row][col_timestamp] = string_timestamp;  /* [][4] */
        ++row;
    }

    /* Successfully parsed */
    status = 0;
    goto end;

end:
    cJSON_Delete(search_json);
    return status;
}

int cwiki_parse_article() {
    int status = 0;

    const cJSON *query = NULL;
    const cJSON *pages = NULL;
    const cJSON *page = NULL;
    char *string_text = NULL;

    cJSON *article_json = cJSON_Parse(cwiki_user_data->url_response_article);

    if (article_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = -1;
        goto end;
    }

    query = cJSON_GetObjectItemCaseSensitive(article_json, "query");
    pages = cJSON_GetObjectItemCaseSensitive(query, "pages");

    cJSON_ArrayForEach(page, pages)
    {
        cJSON *extract = cJSON_GetObjectItemCaseSensitive(page, "extract");

        if (!cJSON_IsString(extract)) {
            status = -1;
            goto end;
        }

        string_text = cJSON_GetStringValue(extract);

        cwiki_user_data->url_response_article_parsed = malloc(strlen(string_text) + 1);
        strcpy(cwiki_user_data->url_response_article_parsed, string_text);
    }

    /* Successfully parsed */
    status = 0;
    goto end;

end:
    cJSON_Delete(article_json);
    return status;

    return 0;
}

int cwiki_parse_html_article() {
    int length = 0;
    xmlDocPtr doc = NULL;
    const char *article = NULL;
    const char *article_root = NULL;

    article = (const char*)cwiki_user_data->url_response_article_parsed;

    article_root = cwiki_parse_html_add_root_to_html(article);
    length = strlen(article_root);

    doc = xmlReadMemory(article_root, length, "noname.xml", NULL, 0);

    if (doc == NULL) {
        fprintf(stderr, "Failed to parse XML\n");
        return -1;
    }

    // cwiki_article_data->headers_count++;
    // xmlNodePtr root = xmlDocGetRootElement(doc);

    // cwiki_parse_count_paragraphs(root);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}

void cwiki_parse_count_paragraphs(xmlNodePtr node) {
    for (xmlNodePtr cur = node; cur; cur = cur->next) {
        if (cur->type == XML_ELEMENT_NODE && xmlStrcmp(cur->name, (const xmlChar*)"p") == 0) {
            xmlChar* content = xmlNodeGetContent(cur);
            // printf("Paragraph: %s\n", content);
            xmlFree(content);
            cwiki_article_data->headers_count++;
        }
        cwiki_parse_count_paragraphs(cur->children);
    }
}

char *cwiki_parse_html_add_root_to_html(const char *html) {

    // Create a new XML document
    xmlDocPtr doc = xmlNewDoc((const xmlChar*)"1.0");
    if (doc == NULL) {
        fprintf(stderr, "Failed to create XML document\n");
        return NULL;
    }

    // Add a root node to the document
    xmlNodePtr root = xmlNewNode(NULL, (const xmlChar*)"root");
    if (root == NULL) {
        fprintf(stderr, "Failed to create root node\n");
        xmlFreeDoc(doc);
        return NULL;
    }
    xmlDocSetRootElement(doc, root);

    // Create a text node containing the HTML string
    xmlNodePtr htmlNode = xmlNewText((const xmlChar*)html);
    if (htmlNode == NULL) {
        fprintf(stderr, "Failed to create HTML node\n");
        xmlFreeDoc(doc);
        return NULL;
    }

    // Add the HTML node as a child of the root node
    xmlAddChild(root, htmlNode);

    // Serialize the XML document to a string
    xmlChar* xmlString;
    int xmlStringSize;
    xmlDocDumpFormatMemory(doc, &xmlString, &xmlStringSize, 1);

    // xmlDocPtr doc = xmlReadMemory(article, length, "noname.xml", NULL, 0);

    // Cleanup and return the XML string
    xmlFreeDoc(doc);
    return (char*)xmlString;
}
