#include "../include/cwiki_log.h"

zlog_category_t *log_debug;

void cwiki_log_init_debug() {
  int rc;

  rc = zlog_init("cwiki_log.conf");

  if (rc) {
    printf("init failed: %d\n", rc);
    log_debug = NULL;
    zlog_fini();
    return;
  }

  log_debug = zlog_get_category("my_cat");

  if (!log_debug) {
    printf("get cat fail\n");
    zlog_fini();
    log_debug = NULL;
    return;
  }

  zlog_info(log_debug, "Logging enabled");
}
