#include <panel.h>
#include <string.h>
#include <stdlib.h>
#include "nlines.h"

/* Public API funtion that handles the creation of VSplitWindow objects.
 *
 * title   - Title of the main window
 * title_l - Title of the left subwindow
 * title_r - Title of the right subwindow
 * width   - The width of the outer window
 * height  - The height of the outer window
 * x       - The x position of the outer window
 * y       - The y position of the outer window
 *
 * return  - A pointer to the created VSplitWindow object
 */
VSplitWindow *
nlines_vswin_create(char *title, char *title_l, char *title_r,
                    int width, int height, int x, int y)
{
    VSplitWindow *vswin;
    
    vswin = calloc(1, sizeof(*vswin));
    // Setting metadata
    strncpy(vswin->meta.title, title, TITLE_SIZE);
    vswin->meta.width = width;
    vswin->meta.height = height;
    vswin->meta.x = x;
    vswin->meta.y = y;
    vswin->meta.type = WIN_VSPLIT;
    // Setting VSplitWindow fields
    vswin->window = newwin(height, width, y, x);
    vswin->left = derwin(vswin->window, height - 2, width / 2 - 2, 1, 1);
    vswin->right = derwin(vswin->window, height - 2, width / 2 - 2, 1,
                          width / 2 + 1);
    vswin->panel  = new_panel(vswin->window);
    strncpy(vswin->title_left, title_l, TITLE_SIZE);
    strncpy(vswin->title_right, title_r, TITLE_SIZE);
    return vswin;
}

/* Public API funtion that handles the creation of BasicWindow objects.
 *
 * title   - Title of the main window
 * width   - The width of the window
 * height  - The height of the window
 * x       - The x position of the window
 * y       - The y position of the window
 *
 * return  - A pointer to the created BasicWindow object
 */
BasicWindow *
nlines_basicwin_create(char *title, int width, int height, int x, int y)
{
    BasicWindow *win;

    win = calloc(1, sizeof(*win));
    // Setting metadata
    strncpy(win->meta.title, title, TITLE_SIZE);
    win->meta.width = width;
    win->meta.height = height;
    win->meta.x = x;
    win->meta.y = y;
    win->meta.type = WIN_BASIC;
    // Setting BasicWindow fields
    win->window = newwin(height, width, y, x);
    win->panel  = new_panel(win->window);
    return win;
}
