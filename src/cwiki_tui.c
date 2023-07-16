#include "../include/cwiki_tui.h"
// #include "../include/cwiki_log.h"

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
	mvwprintw(local_win, 0, (width * 0.5) - strlen(header) + 4, "%s", header);
	wattroff(local_win, A_BOLD | COLOR_PAIR(1));

	/* Display window */
	wrefresh(local_win);

	return local_win;
}

void cwiki_tui_window_search() {
	const char *header = "| Search |";
	char search[52];
	/* Center the search window */
	WINDOW *window_search = cwiki_tui_window_create(WINDOW_SEARCH_HEIGHT, WINDOW_SEARCH_WIDTH,
													cwiki_tui_screen_height/5 + (cwiki_tui_screen_height / 6),
													cwiki_tui_screen_width/4, header);

	echo();
	curs_set(1);

	/* NOTE: only first word of user input is stored in search */
	/* TODO: Make whole sentence get stored in search */
	/* NOTE: Maybe doesn't matter if I use pageid to get article later in the program? */
	mvwscanw(window_search, 1, 1, "%s", search);

	noecho();
	curs_set(0);

	cwiki_user_data->search = malloc(sizeof search + 1);
	strcpy(cwiki_user_data->search, search);

	delwin(window_search);
}

void cwiki_tui_window_articles() {
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
	strcat(header, cwiki_user_data->search);
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
				cwiki_user_data->selected_article_title = malloc(strlen(item_name(current_item(menu_articles))) + 1);

				cwiki_user_data->selected_article_title = (char *)item_name(current_item(menu_articles));
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

void cwiki_tui_window_article_view() {
	WINDOW *window_article_view, *window_article_toc, *window_focus;
	PANEL  *panel_article_view, *panel_article_toc, *panel_focus;
	int article_view_height, article_view_width;
	int c;
	char header_article[52] = {0};
	const char *header_toc = "| toc |";

	clear();

	strcat(header_article, "| ");
	strcat(header_article, cwiki_user_data->selected_article_title);
	strcat(header_article, " |");

	/* Create windows for the panels */
	window_article_view = cwiki_tui_window_create(cwiki_tui_screen_height, cwiki_tui_screen_width * 0.75,
												  0, 1, header_article);

	getmaxyx(window_article_view, article_view_height, article_view_width);
	(void)article_view_height;

	window_article_toc = cwiki_tui_window_create(cwiki_tui_screen_height, cwiki_tui_screen_width * 0.25,
												  0, article_view_width, header_toc);

	/* Attach a panel to each window */     /* Order is bottom up */
	panel_article_view = new_panel(window_article_view);   /* Push 0, order: stdscr−0 */
	panel_article_toc = new_panel(window_article_toc);   /* Push 0, order: stdscr−0-1 */

   /* Set up the user pointers to the next panel */
   set_panel_userptr(panel_article_view, panel_article_toc);
   set_panel_userptr(panel_article_toc, panel_article_view);

	/* TODO: Show the user which panel is currently in focus */
   panel_focus = panel_article_view;

   /* Set current window as focused */
	window_focus = panel_window(panel_focus);
	cwiki_tui_window_focus(window_focus, TRUE);

	/* Update the stacking order. 2nd panel will be on top */
	update_panels();

	/* Show it on the screen */
	doupdate();

	while( (c = wgetch(stdscr)) != 'q' ) {
		switch(c) {
			/* Tab to alternate focus between article and toc */
			case 9:
				/* Unfocus current window */
				cwiki_tui_window_focus(window_focus, FALSE);

				panel_focus = (PANEL *)panel_userptr(panel_focus);
				top_panel(panel_focus);
				window_focus = panel_window(panel_focus);

				/* Set newly focused window to focus */
				cwiki_tui_window_focus(window_focus, TRUE);

				break;
		}
		update_panels();
		doupdate();
	}

	del_panel(panel_article_view);
	del_panel(panel_article_toc);

	delwin(window_article_view);
	delwin(window_article_toc);
}

void cwiki_tui_window_focus(WINDOW *local_win, bool focus) {
	int height, width;
	// int vline_y = 1;
	// int hline_x = 4;
	init_pair(1, COLOR_RED, COLOR_BLACK);

	getmaxyx(local_win, height, width);

	if (focus) {
		wattron(local_win, COLOR_PAIR(1));
		/* Print lines on left, right and bottom of window */
		// mvwvline(local_win, vline_y, 0, 0, height - 1); /* left side */
		// mvwvline(local_win, vline_y, width - 1, 0, height - 1); /* right side */

		mvwhline(local_win, height - 1, width * 0.1, 0, width * 0.8); /* bottom side*/
		wattroff(local_win, COLOR_PAIR(1));

		return;
	}

	/* If window is not focused then print lines without colors */
	// mvwvline(local_win, vline_y, 0, 0, height - 1); /* left side */
	// mvwvline(local_win, vline_y, width - 1, 0, height - 1); /* right side */

	mvwhline(local_win, height - 1, width * 0.1, 0, width * 0.8); /* bottom side*/

	wrefresh(local_win);
}
