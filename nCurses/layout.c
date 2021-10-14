#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curses.h>

int main(int argc, char *argv[])
{
    WINDOW *std;
    WINDOW *stack;
    char title[] = "Welcome to Stack Visualizer v0.1";

	initscr();
    curs_set(0);
    cbreak();
    noecho();
    start_color();

    // Init colors
    init_pair(1, COLOR_CYAN, COLOR_BLACK);

    // Init windows
    std   = newwin(LINES - 1, COLS / 2, 1, 0);
    stack = newwin(LINES - 1, COLS / 2 - 1, 1, (COLS / 2) + 1);

    // Bordering windows
    box(std, '|', '-');
    box(stack, '|', '-');

    // Printing to windows
    mvwprintw(std, 1, 1, "This is program stdout"); 
    mvwprintw(stack, 1, 1, "This is program stack dumps"); 

    // Printing title to stdscr
    mvprintw(0, (COLS/2) - (strlen(title)/2), "%s", title);
    // Setting attributes of title
    mvchgat(0, 0, -1, A_BOLD, 1, NULL);

    // Refreshing
    wnoutrefresh(stdscr);
    wnoutrefresh(std);
    wnoutrefresh(stack);
    doupdate();

    // Wait for input before closing
    getch();

    delwin(std);
    delwin(stack);
    endwin();
	return EXIT_SUCCESS;	
}

