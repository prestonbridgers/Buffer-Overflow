#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <panel.h>

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

    // Curses initialization
    initscr();
    cbreak();
    noecho();

    // Init windows
    w1 = newwin(winLines, winColumns, begY, begX);
    w2 = newwin(winLines, winColumns, begY+5, begX);
    w3 = newwin(winLines, winColumns, begY+10, begX);

    // Adding borders to the windows
    box(w1, '|', '-');
    box(w2, '|', '-');
    box(w3, '|', '-');

    // Printing unique strings to each window
    mvwaddstr(w1, 1, 1, "This is window 1");
    mvwaddstr(w2, 1, 1, "This is window 2");
    mvwaddstr(w3, 1, 1, "This is window 3");

    // Init panels
    p3 = new_panel(w3); // Done in reverse b/c each call updates top_panel
    p2 = new_panel(w2);
    p1 = new_panel(w1); // Panel Order: p1 -> p2 -> p3

    // Setting panel user pointers
    // Each panel's user pointer is just a pointer to anything I want.
    // In this case, it's a pointer to the next window.
    set_panel_userptr(p1, p2);
    set_panel_userptr(p2, p3);
    set_panel_userptr(p3, p1);

    update_panels(); // Updates virtual screen to reflect panel stack.
    doupdate(); // Updates the physical screen (what is actually displayed).

    // Loop for tabbing through panels.
    // TAB -> Brings the next panel to the top.
    // q   -> Breaks the loop and exits program.
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

    // Freeing memory associated with windows.
    delwin(w1);
    delwin(w2);
    delwin(w3);

    // Freeing memory associated with panels.
    del_panel(p1);
    del_panel(p2);
    del_panel(p3);

    // Exiting curses mode
    endwin();
	return EXIT_SUCCESS;	
}

