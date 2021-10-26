#ifndef INLINES_INCLUDED
#define INLINES_INCLUDED

#include <panel.h>

#define TITLE_SIZE 128

#define NLINES_CENTER_COLS(x) ((COLS / 2) - (x / 2))
#define NLINES_CENTER_LINES(x) ((LINES / 2) - (x / 2))

typedef enum
{
    WIN_BASIC,
    WIN_VSPLIT,
    WIN_COUNT
} WindowType;

typedef struct
{
    char title[TITLE_SIZE];
    int width;
    int height;
    int x;
    int y;
    WindowType type;
} BaseWindow;

typedef struct
{
    BaseWindow meta;
    WINDOW *window;
    WINDOW *left;
    WINDOW *right;
    PANEL *panel;
    char title_right[TITLE_SIZE];
    char title_left[TITLE_SIZE];
} VSplitWindow;

typedef struct
{
    BaseWindow meta;
    WINDOW *window;
    PANEL *panel;
} BasicWindow;

// External/Public API
VSplitWindow *nlines_vswin_create(char *title, char *title_l, char *title_r,
                                  int width, int height, int x, int y);
BasicWindow *nlines_basicwin_create(char *title, int width, int height,
                                    int x, int y);
void nlines_destroy(BaseWindow *base);
void nlines_draw(BaseWindow *base, void* (*draw_func)(void*));
#endif
