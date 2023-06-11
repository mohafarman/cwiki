#ifndef CWIKI_LOG_H_
#define CWIKI_LOG_H_

#include "zlog.h"

void cwiki_log_init_debug();

extern zlog_category_t *log_debug;

#endif // CWIKI_LOG_H_
