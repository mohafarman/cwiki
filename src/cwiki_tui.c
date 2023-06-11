#include "../include/cwiki_tui.h"


void init_ncurses() {
	initscr();
	start_color();
	cbreak();
	noecho();

	/* Enable support for function-key mapping */
	keypad(stdscr, TRUE);
	curs_set(0);
}

char *test(void) {
	return "Hello, Worldaa!\n";
}
