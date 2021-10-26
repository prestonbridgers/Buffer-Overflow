#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include "nlines.h"

// GLOBALS
uint64_t *ret_ptr;
uint64_t *stack_ptr;
uint64_t *buf_ptr;

// MACROS
#define BEFORE_UNSAFE_CALL() \
    asm volatile ( \
        "mov $1, %%r8;" \
        "not %%r8;" \
        "inc %%r8;" \
        "lea (%%rsp, %%r8, 8), %%r9;" \
        "mov %%r9, %0;" \
        : "=rm" (ret_ptr) );

#define GET_STACK_PTR() \
    asm volatile ( \
        "mov %%rsp, %0;" \
        : "=rm" (stack_ptr) );

#define GET_BUF_PTR(buf) \
    buf_ptr = (uint64_t*)buf;

#define NUM_LINES 10

/* Prints a single 8 byte word to stderr:
 *
 * Ex:
 * 0x0007ffffffffe80c: 0x0f0e0d0c0b0a0908
 * 
 * win      - The window to which to draw the stack
 * line_ptr - The address to print
 * ypos     - The y position relative to win's origin to draw the lines.
 */
void print_line(BasicWindow *win, uint64_t *line_ptr, int ypos)
{
    mvwprintw(win->window, ypos + 2, 2, "%#018" PRIx64 ": %#018" PRIx64, (uint64_t)line_ptr, *line_ptr);
    if (line_ptr == buf_ptr)
    {
        wprintw(win->window, " <- Buffer");
    }
    else if (line_ptr == stack_ptr)
    {
        wprintw(win->window, " <- %%RSP");
    }
    else if (line_ptr == ret_ptr)
    {
        wprintw(win->window, " <- Ret. Addr.");
    }
    wprintw(win->window, "\n");
    box(win->window, 0, 0);
    mvwaddnstr(win->window, 0,
               (win->meta.width / 2) - (strlen(win->meta.title) / 2),
               win->meta.title, TITLE_SIZE);
    mvwprintw(win->window, win->meta.height - 2, 2,
              "Press 'q' to quit the program once the visualization has finished");
}

/* Prints 8 byte lines from the stack starting at address stack_ptr.
 *
 * nlines - The number of lines to print
 * done   - 1 or 0 that determines whether to print an escape sequence that
 *          brings the cursor back up to the start of the stack that has been
 *          printed. Using this prevents the prompt from being printed inside
 *          the stack after program completion.
 * win    - The window to which to draw the stack.
 */
void print_stack(BasicWindow *win)
{
    uint64_t *tmp = stack_ptr;
    int i = 0;
    for (i = 0; i < NUM_LINES; i++)
    {
        print_line(win, tmp, i);
        tmp++;
    }
    update_panels();
    doupdate();
    return;
}

/* Allocates a 16 byte buffer on the stack, sets each byte printing the stack
 * after each byte is set. A delay is used so that you can visually see the
 * stack setting each byte of the buffer.
 *
 * win - The window to which to draw the stack.
 */
uint8_t foo(BasicWindow *win) {
    GET_STACK_PTR();
    uint8_t my_buffer[16];
    GET_BUF_PTR(my_buffer);

    int i = 0;
    for (i = 0; i < 16; i++)
    {
        my_buffer[i] = i;
        print_stack(win);
        usleep(500000);
    }

    return my_buffer[0];
}

/* Entry point of the program. Uses BEFORE_UNSAFE_CALL macro to set global
 * ret_ptr to the address of the return address, the instruction after the call
 * to foo.
 */
int main(int argc, char *argv[])
{
    int win_width, win_height;
    BasicWindow *win;

    initscr();
    curs_set(0);

    win_width = COLS / 2;
    win_height = LINES / 2;
    win = nlines_basicwin_create("Memory Visualization v0.1", win_width,
                                 win_height, NLINES_CENTER_COLS(win_width),
                                 NLINES_CENTER_LINES(win_height));

    BEFORE_UNSAFE_CALL();
    foo(win);

    getch();
    nlines_destroy((BaseWindow*)win);
    endwin();
    return EXIT_SUCCESS;	
}

