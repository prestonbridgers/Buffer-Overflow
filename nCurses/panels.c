#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <panel.h>

int curses_init()
{
    initscr();
    cbreak();
    noecho();
    return EXIT_SUCCESS;
}

int curses_quit()
{
    endwin();
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    WINDOW *w1;
    WINDOW *w2;
    WINDOW *w3;
    PANEL *p1;
    PANEL *p2;
    PANEL *p3;
    PANEL *top;
    int winLines = 10;
    int winColumns  = 30;
    int begY = 10;
    int begX = 10;
    int ch;

    curses_init();

    // Init windows
    w1 = newwin(winLines, winColumns, begY, begX);
    w2 = newwin(winLines, winColumns, begY+5, begX);
    w3 = newwin(winLines, winColumns, begY+10, begX);

    box(w1, '|', '-');
    box(w2, '|', '-');
    box(w3, '|', '-');

    mvwaddstr(w1, 1, 1, "This is window 1");
    mvwaddstr(w2, 1, 1, "This is window 2");
    mvwaddstr(w3, 1, 1, "This is window 3");

    // Init panels
    p3 = new_panel(w3); // Done in reverse b/c each call updates top_panel
    p2 = new_panel(w2);
    p1 = new_panel(w1); // Panel Order: p1 -> p2 -> p3

    set_panel_userptr(p1, p2); // Sets user pointers
    set_panel_userptr(p2, p3);
    set_panel_userptr(p3, p1);

    update_panels(); // Updates virtual screen to reflect panel stack.
    doupdate(); // Updates the physical screen (what is actually displayed).

    top = p1;
    while ((ch = getch()) != 'q')
    {
        switch (ch)
        {
            case 9: // 9 is ASCII code for a TAB character
                top = (PANEL *) panel_userptr(top);
                top_panel(top);
                break;
        }
        update_panels();
        doupdate();
    }

    delwin(w1);
    delwin(w2);
    delwin(w3);

    del_panel(p1);
    del_panel(p2);
    del_panel(p3);
    curses_quit();
	return EXIT_SUCCESS;	
}

