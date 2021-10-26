#include <stdlib.h>
#include "nlines.h"

static void nlines_basicwin_destroy_internal(BasicWindow *win);
static void nlines_vswin_destroy_internal(VSplitWindow *win);

/* Public API function that frees memory associated with a window.
 *
 * base - A window that mustbe cast as a BaseWindow for inheritence
 *
 * Example usage:
 *    nlines_destroy((BaseWindow*)my_vsplit_window);
 */
void
nlines_destroy(BaseWindow *base)
{
    switch(base->type) {
        case WIN_VSPLIT:
            nlines_vswin_destroy_internal((VSplitWindow*)base);
            break;
        case WIN_BASIC:
            nlines_basicwin_destroy_internal((BasicWindow*)base);
            break;
        default:
            break;
    }
}

/* Frees memory associated with given BasicWindow struct.
 *
 * win - The BasicWindow struct to destroy
 */
static void
nlines_basicwin_destroy_internal(BasicWindow *win)
{
    del_panel(win->panel);
    delwin(win->window);
    free(win);
    return;
}

/* Frees memory associated with given VSplitWindow struct.
 *
 * win - The VSplitWindow struct to destroy
 */
static void
nlines_vswin_destroy_internal(VSplitWindow *win)
{
    del_panel(win->panel);
    delwin(win->window);
    delwin(win->right);
    delwin(win->left);
    free(win);
    return;
}
