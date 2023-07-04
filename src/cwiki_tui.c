#include "../include/cwiki.h"
#include "../include/cwiki_tui.h"
#include "../include/cwiki_log.h"

#include <menu.h>
#include <stdlib.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define NUM_ARTICLES 10

int cwiki_tui_screen_height;
int cwiki_tui_screen_width;

void cwiki_tui_init_ncurses() {
	initscr();
	start_color();
	cbreak();

	/* Prevent keyboard input echoing */
	noecho();

	/* Enable support for function-key mapping */
	keypad(stdscr, TRUE);

	curs_set(0);

	/* Get max screen height and width */
	getmaxyx(stdscr, cwiki_tui_screen_height, cwiki_tui_screen_width);

	cwiki_tui_screen_clear();

	zlog_info(log_debug, "ncurses initialized");
}

void cwiki_tui_screen_decorate() {
	char *header = "| cwiki |";

	/* Create a border */
	box(stdscr, 0, 0);

	/* Print header */
	wattron(stdscr, A_BOLD);
	mvwprintw(stdscr, 0, cwiki_tui_screen_width/2 - strlen(header), "%s", header);
	wattroff(stdscr, A_BOLD);
}

void cwiki_tui_window_redraw(WINDOW *local_win, const char *header) {
	int height, width;

	getmaxyx(local_win, height, width);
	(void)height;

	/* Create a border */
	box(local_win, 0, 0);

	/* Print header */
	wattron(local_win, A_BOLD);
	mvwprintw(local_win, 0, width/2 - 4, "%s", header);
	wattroff(local_win, A_BOLD);

	wrefresh(local_win);
}

void cwiki_tui_screen_clear() {
	clear();
	cwiki_tui_screen_decorate();
	refresh();
}

WINDOW *cwiki_tui_window_create(int height, int width, int start_y, int start_x, const char *header) {
	WINDOW *local_win = NULL;
	keypad(local_win, TRUE);
	local_win = newwin(height, width, start_y, start_x);
	box(local_win, 0, 0);

	/* Print header */
	wattron(local_win, A_BOLD);
	/* 4 magic number for offset */
	mvwprintw(local_win, 0, width/2 - strlen(header) + 4, "%s", header);
	wattroff(local_win, A_BOLD);

	/* Display window */
	wrefresh(local_win);

	return local_win;
}

void cwiki_tui_window_search() {
	const char *header = "| Search |";
	/* Center the search window */
	WINDOW *window_search = cwiki_tui_window_create(WINDOW_SEARCH_HEIGHT, WINDOW_SEARCH_WIDTH, cwiki_tui_screen_height/4, cwiki_tui_screen_width/4, header);

	echo();
	curs_set(1);

	mvwscanw(window_search, 1, 1, "%s", cwiki_user_data->text_search);

	noecho();
	curs_set(0);

	delwin(window_search);
}

void cwiki_tui_window_articles(cwiki_user_s* cwiki_user_data) {
	/* use the menu.h from ncurses for the user to select and view snippets of the articles */
	ITEM **items_articles;
	MENU *menu_articles;
	int num_article = 0;
	/* enough height to view 10 articles, enough width to show most article titles */
	int menu_height = 12, menu_width = 40;
	int height, width;
	int starty, startx;
	// int window_snippets_width_padding = 4;
	int c;

	const char *header = "| Results |";

	const char *header_preview = "| Preview |";
	/* Center the search window */
	/* TODO: Figure out a way to dynamically calcute center of the screen */
	WINDOW *window_articles = cwiki_tui_window_create(cwiki_tui_screen_height/1.6, cwiki_tui_screen_width/1.6,
													  cwiki_tui_screen_height/5, cwiki_tui_screen_width/5,
													  header);

	getbegyx(window_articles, starty, startx);
	getmaxyx(window_articles, height, width);

	(void)startx;

	/* Window to contain the window_snippets */
	WINDOW *window_preview = cwiki_tui_window_create(
		height / 2, width/2,
		starty + 1, (startx + width)/1.7,
		header_preview);

	getbegyx(window_preview, starty, startx);
	getmaxyx(window_preview, height, width);

	/* Window for displaying snippets */
	WINDOW *window_snippets = newwin(
		height - 4, width - 4,
		starty + 2, startx + 2);

	/* Create items */
	items_articles = (ITEM **)calloc(NUM_ARTICLES, sizeof(ITEM *));
	for (int i = 0; i < NUM_ARTICLES; i++) {
		items_articles[i] = new_item(cwiki_user_data->url_response_parsed[i][0], "");
	}

	/* Create menu */
	menu_articles = new_menu((ITEM **)items_articles);

	set_menu_win(menu_articles, window_articles);
	set_menu_sub(menu_articles, derwin(window_articles, menu_height, menu_width, 3, 1));

	/* Set menu mark */
	set_menu_mark(menu_articles, " > ");

	post_menu(menu_articles);
	wrefresh(window_articles);
	wrefresh(window_preview);

	mvwprintw(window_snippets, 0, 0, cwiki_user_data->url_response_parsed[num_article][2]);
	wrefresh(window_snippets);

	while((c = wgetch(window_articles)) != 'q')
	{
		switch(c)
		{
			/* VIM keybindings */
			case 'j':
				menu_driver(menu_articles, REQ_DOWN_ITEM);

				/* Limit to number of articles */
				if (num_article < NUM_ARTICLES - 1) {
					num_article++;
				}
				wclear(window_snippets);
				mvwprintw(window_snippets, 0, 0, cwiki_user_data->url_response_parsed[num_article][2]);
				wrefresh(window_snippets);
				break;
			case 'k':
				menu_driver(menu_articles, REQ_UP_ITEM);

				if (num_article > 0) {
					num_article--;
				}
				wclear(window_snippets);
				mvwprintw(window_snippets, 0, 0, cwiki_user_data->url_response_parsed[num_article][2]);
				wrefresh(window_snippets);
				break;
			case 10: /* Return */
				/*  Select an item */
				cwiki_user_data->selected_article_title = item_name(current_item(menu_articles));
				goto end;
		}
	}

end:
	/* Unpost and free all the memory taken up */
	unpost_menu(menu_articles);
	free_menu(menu_articles);
	for(int i = 0 ; i < NUM_ARTICLES; ++i)
			free_item(items_articles[i]);
	delwin(window_articles);
	delwin(window_preview);
	delwin(window_snippets);
	cwiki_tui_screen_clear();
}
