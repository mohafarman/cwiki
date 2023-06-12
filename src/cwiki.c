#include "../include/cwiki_tui.h"
#include "../include/cwiki_log.h"

#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    char c;

    // 1. Logging
    cwiki_log_init_debug();

    /* Setup ncurses */
    init_ncurses();

    refresh();

	while ( (c = getch()) != 'q' ) {

    }

	endwin();
    zlog_info(log_debug, "Terminating application");
    return 0;
}
