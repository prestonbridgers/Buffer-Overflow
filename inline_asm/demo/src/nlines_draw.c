#include <string.h>
#include <nlines.h>

static void nlines_basicwin_draw_internal(BasicWindow *win);
static void nlines_vswin_draw_internal(VSplitWindow *win);

/* Public API function that handles drawing windows to stdscr.
 *
 * base - A window that must be cast as a BaseWindow for inheritence.
 *
 * Example usage:
 *    nlines_draw((BaseWindow*)my_vsplit_window);
 */
void
nlines_draw(BaseWindow *base, void* (*draw_func)(void*))
{
    switch (base->type) {
        case WIN_VSPLIT:
            nlines_vswin_draw_internal((VSplitWindow*)base);
            break;
        case WIN_BASIC:
            nlines_basicwin_draw_internal((BasicWindow*)base);
            break;
        default:
            break;
    }
    draw_func(base);
    return;
}

/* Internal function for drawing VSplitWindows.
 *
 * win - Pointer to the VSplitWindow to draw
 */
static void
nlines_vswin_draw_internal(VSplitWindow *win)
{
    int width, height;

    box(win->window, 0, 0);
    mvwaddnstr(win->window, 0,
               (win->meta.width / 2) - (strlen(win->meta.title) / 2),
               win->meta.title, TITLE_SIZE);
    getmaxyx(win->left, height, width);
    box(win->left, 0, 0);
    mvwaddnstr(win->left, height - height,
               (width / 2) - (strlen(win->title_left) / 2),
               win->title_left, TITLE_SIZE);
    getmaxyx(win->right, height, width);
    box(win->right, 0, 0);
    mvwaddnstr(win->right, height - height,
               (width / 2) - (strlen(win->title_right) / 2),
               win->title_right, TITLE_SIZE);
    return;
}

/* Internal function for drawing BasicWindows.
 *
 * win - Pointer to the BasicWindow to draw
 */
static void
nlines_basicwin_draw_internal(BasicWindow *win)
{
    box(win->window, 0, 0);
    mvwaddnstr(win->window, 0,
               (win->meta.width / 2) - (strlen(win->meta.title) / 2),
               win->meta.title, TITLE_SIZE);
    return;
}
