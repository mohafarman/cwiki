#include "../include/cwiki.h"
#include "../include/cwiki_tui.h"
#include "../include/cwiki_log.h"

int cwiki_tui_screen_height;
int cwiki_tui_screen_width;

void cwiki_tui_init_ncurses() {
	char *header = "cwiki";
	initscr();
	start_color();
	cbreak();

	/* Prevent keyboard input echoing */
	noecho();

	/* Enable support for function-key mapping */
	keypad(stdscr, TRUE);

	curs_set(0);

	/* Create a border */
	box(stdscr, 0, 0);

	/* Get max screen height and width */
	getmaxyx(stdscr, cwiki_tui_screen_height, cwiki_tui_screen_width);

	/* Print header */
	wattron(stdscr, A_BOLD);
	mvwprintw(stdscr, 0, cwiki_tui_screen_width/2 - strlen(header), "%s", header);
	wattroff(stdscr, A_BOLD);

	zlog_info(log_debug, "ncurses initialized");

	refresh();
}

WINDOW *cwiki_tui_window_create(int height, int width, int start_y, int start_x) {
	WINDOW *local_win = NULL;
	keypad(local_win, TRUE);
	local_win = newwin(height, width, start_y, start_x);
	box(local_win, 0, 0);

	/* Display window */
	wrefresh(local_win);

	return local_win;
}

void cwiki_tui_window_search() {
	/* Center the search window */
	WINDOW *window_search = cwiki_tui_window_create(WINDOW_SEARCH_HEIGHT, WINDOW_SEARCH_WIDTH, cwiki_tui_screen_height/4, cwiki_tui_screen_width/4);

	echo();
	curs_set(1);

	mvwscanw(window_search, 1, 1, "%s", cwiki_user_data->text_search);

	noecho();
	curs_set(0);
}
