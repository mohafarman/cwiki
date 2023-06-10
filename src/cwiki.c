#include "../include/tui.h"

#include <stdio.h>
#include <math.h>
#include <ncurses.h>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    // init_ncurses();
    initscr();

    puts(test());

    printf("Pi: %f\n", M_PI);

    endwin();
    return 0;
}
