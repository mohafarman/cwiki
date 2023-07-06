#ifndef TUI_H_
#define TUI_H_

#include "cwiki.h"

#include <ncurses.h>
#include <menu.h>
#include <string.h>

#define WINDOW_MAIN_HEIGHT 20
#define WINDOW_MAIN_WIDTH 80
#define WINDOW_MAIN_START_Y 0
#define WINDOW_MAIN_START_X 0

#define WINDOW_SEARCH_HEIGHT 3
#define WINDOW_SEARCH_WIDTH 80
#define WINDOW_SEARCH_START_Y 0
#define WINDOW_SEARCH_START_X 0

extern int cwiki_tui_screen_height;
extern int cwiki_tui_screen_width;

void cwiki_tui_init_ncurses();

void cwiki_tui_screen_decorate();
void cwiki_tui_window_redraw(WINDOW*, const char*);
void cwiki_tui_screen_clear();

WINDOW *cwiki_tui_window_create(int, int, int, int, const char*);

void cwiki_tui_window_search();
void cwiki_tui_window_articles(cwiki_user_s*);

#endif // TUI_H_
