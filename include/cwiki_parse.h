#ifndef CWIKI_PARSE_H_
#define CWIKI_PARSE_H_

#include "../include/cwiki.h"

#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>

int cwiki_parse_search();

int cwiki_parse_article();

int cwiki_parse_html_article();

void cwiki_parse_count_paragraphs(xmlNodePtr);

#endif // CWIKI_PARSE_H_
