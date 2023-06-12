#include "../include/cwiki_tui.h"
#include "../include/cwiki_log.h"


void init_ncurses() {
	initscr();
	start_color();
	cbreak();

	/* Prevent keyboard input echoing */
	noecho();

	/* Enable support for function-key mapping */
	keypad(stdscr, TRUE);

	curs_set(0);

	zlog_info(log_debug, "ncurses initialized");
	fprintf(stdout, "Hello from init_ncurses()\n");

	fflush(stdout);
}

char *test(void) {
	return "Hello, Worldaa!\n";
}
