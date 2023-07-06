#include "../include/cwiki.h"
#include "../include/cwiki_tui.h"
#include "../include/cwiki_log.h"

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
	init_pair(1, COLOR_RED, COLOR_BLACK);

	/* Create a border */
	box(stdscr, 0, 0);

	/* Print header */
	wattron(stdscr, A_BOLD | COLOR_PAIR(1));
	mvwprintw(stdscr, 0, cwiki_tui_screen_width/2 - strlen(header), "%s", header);
	wattroff(stdscr, A_BOLD | COLOR_PAIR(1));
}

void cwiki_tui_screen_logo() {
	init_pair(1, COLOR_RED, COLOR_BLACK);
	/* Print logo */
	wattron(stdscr, A_BOLD | COLOR_PAIR(1));
	mvwprintw(stdscr, 3, cwiki_tui_screen_width/2.5, "               _ _    _  ");
	mvwprintw(stdscr, 4, cwiki_tui_screen_width/2.5, "              (_) |  (_) ");
	mvwprintw(stdscr, 5, cwiki_tui_screen_width/2.5, "  _____      ___| | ___  ");
	mvwprintw(stdscr, 6, cwiki_tui_screen_width/2.5, " / __\\ \\ /\\ / / | |/ / | ");
	mvwprintw(stdscr, 7, cwiki_tui_screen_width/2.5, "| (__ \\ V  V /| |   <| | ");
	mvwprintw(stdscr, 8, cwiki_tui_screen_width/2.5, " \\___| \\_/\\_/ |_|_|\\_\\_| ");
	wattroff(stdscr, A_BOLD | COLOR_PAIR(1));
}

void cwiki_tui_window_redraw(WINDOW *local_win, const char *header) {
	int height, width;
	init_pair(1, COLOR_RED, COLOR_BLACK);

	getmaxyx(local_win, height, width);
	(void)height;

	/* Create a border */
	box(local_win, 0, 0);

	/* Print header */
	wattron(local_win, A_BOLD | COLOR_PAIR(1));
	mvwprintw(local_win, 0, width/2 - 4, "%s", header);
	wattroff(local_win, A_BOLD | COLOR_PAIR(1));

	wrefresh(local_win);
}

void cwiki_tui_screen_clear() {
	clear();
	cwiki_tui_screen_decorate();
	refresh();
}

WINDOW *cwiki_tui_window_create(int height, int width, int start_y, int start_x, const char *header) {
	WINDOW *local_win = NULL;

	init_pair(1, COLOR_RED, COLOR_BLACK);

	keypad(local_win, TRUE);
	local_win = newwin(height, width, start_y, start_x);
	box(local_win, 0, 0);

	/* Print header */
	wattron(local_win, A_BOLD | COLOR_PAIR(1));
	/* 4 magic number for offset */
	mvwprintw(local_win, 0, width/2 - strlen(header) + 4, "%s", header);
	wattroff(local_win, A_BOLD | COLOR_PAIR(1));

	/* Display window */
	wrefresh(local_win);

	return local_win;
}

void cwiki_tui_window_search() {
	const char *header = "| Search |";
	/* Center the search window */
	WINDOW *window_search = cwiki_tui_window_create(WINDOW_SEARCH_HEIGHT, WINDOW_SEARCH_WIDTH,
													cwiki_tui_screen_height/5 + (cwiki_tui_screen_height / 6),
													cwiki_tui_screen_width/4, header);

	echo();
	curs_set(1);

	mvwscanw(window_search, 1, 1, "%s", cwiki_user_data->text_search);

	noecho();
	curs_set(0);

	delwin(window_search);
}

void cwiki_tui_window_articles(cwiki_user_s* cwiki_user_data) {
	ITEM **items_articles;
	MENU *menu_articles;
	int num_article = 0;
	/* enough height to view 10 articles, enough width to show most article titles */
	int menu_height = 12, menu_width = 40;
	int articles_height, articles_width;
	int articles_starty, articles_startx;
	int preview_height, preview_width;
	int preview_starty, preview_startx;
	int info_height, info_width;
	int info_starty, info_startx;
	int c;
	char header[52] = {0};

	// const char *header = "| Results |";
	strcat(header, "| Results for '");
	strcat(header, cwiki_user_data->text_search);
	strcat(header, "' |");

	const char *header_preview = "| Preview |";
	const char *header_info = "| Info |";
	/* Center the search window */
	/* TODO: Figure out a way to dynamically calcute center of the screen */
	WINDOW *window_articles = cwiki_tui_window_create(cwiki_tui_screen_height/2, cwiki_tui_screen_width/1.6,
													  cwiki_tui_screen_height/5, cwiki_tui_screen_width/5,
													  header);

	getbegyx(window_articles, articles_starty, articles_startx);
	getmaxyx(window_articles, articles_height, articles_width);

	/* Window to contain window_preview_text */
	WINDOW *window_preview = cwiki_tui_window_create(
		articles_height / 2, articles_width / 2,
		articles_starty + 1, (articles_startx + articles_width) / 1.7,
		header_preview);

	getbegyx(window_preview, preview_starty, preview_startx);
	getmaxyx(window_preview, preview_height, preview_width);

	/* Window for preview text */
	WINDOW *window_preview_text = newwin(
		preview_height - 4, preview_width - 4,
		preview_starty + 2, preview_startx + 2);

	/* Window to contain window_info_text */
	WINDOW *window_info = cwiki_tui_window_create(
		(articles_height / 2) - 1, articles_width / 2,
		preview_starty + preview_height, (articles_startx + articles_width) / 1.7,
		header_info);

	getbegyx(window_info, info_starty, info_startx);
	getmaxyx(window_info, info_height, info_width);

	/* Window for displaying info text */
	WINDOW *window_info_text = newwin(
		info_height - 4, info_width - 4,
		info_starty + 1, info_startx + 2);

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

	wnoutrefresh(window_articles);
	wnoutrefresh(window_preview);
	wnoutrefresh(window_info);

	/* Print the preview_text of the first entry already */
	mvwprintw(window_preview_text, 0, 0, cwiki_user_data->url_response_parsed[num_article][2]);
	wnoutrefresh(window_preview_text);

	mvwprintw(window_info_text, 0, 0, "Pages: %s", cwiki_user_data->url_response_parsed[num_article][3]); /* Print word count */
	mvwprintw(window_info_text, 1, 0, "Edited: %s", cwiki_user_data->url_response_parsed[num_article][4]); /* Print timestamp */
	wnoutrefresh(window_info_text);

	doupdate();

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
				wclear(window_preview_text);
				wclear(window_info_text);
				mvwprintw(window_preview_text, 0, 0, cwiki_user_data->url_response_parsed[num_article][2]);
				mvwprintw(window_info_text, 0, 0, "Pages: %s", cwiki_user_data->url_response_parsed[num_article][3]); /* Print word count */
				mvwprintw(window_info_text, 1, 0, "Edited: %s", cwiki_user_data->url_response_parsed[num_article][4]); /* Print timestamp */
				wnoutrefresh(window_preview_text);
				wnoutrefresh(window_info_text);

				doupdate();
				break;
			case 'k':
				menu_driver(menu_articles, REQ_UP_ITEM);

				if (num_article > 0) {
					num_article--;
				}
				wclear(window_preview_text);
				wclear(window_info_text);
				mvwprintw(window_preview_text, 0, 0, cwiki_user_data->url_response_parsed[num_article][2]);
				mvwprintw(window_info_text, 0, 0, "Pages: %s", cwiki_user_data->url_response_parsed[num_article][3]); /* Print word count */
				mvwprintw(window_info_text, 1, 0, "Edited: %s", cwiki_user_data->url_response_parsed[num_article][4]); /* Print timestamp */
				wnoutrefresh(window_preview_text);
				wnoutrefresh(window_info_text);

				doupdate();
				break;
			case 10: /* Return */
				/*  Select an item */
				cwiki_user_data->selected_article_title = item_name(current_item(menu_articles));
				cwiki_user_data->selected_article_pageid = atoi(cwiki_user_data->url_response_parsed[num_article][2]);
				goto end;
		}

		if (c == 'q') {
			goto end;
		}
	}

end:
	/* TODO: Figure out if this runs when user quits the while loop, 'q' */
	/* Unpost and free all the memory taken up */
	unpost_menu(menu_articles);
	free_menu(menu_articles);
	for(int i = 0 ; i < NUM_ARTICLES; ++i)
			free_item(items_articles[i]);
	delwin(window_articles);
	delwin(window_preview);
	delwin(window_preview_text);
	delwin(window_info);
	delwin(window_info_text);
	cwiki_tui_screen_clear();
}
