#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <curses.h>
#include <panel.h>
#include <pthread.h>

/******************************************************************
 *                          GLOBALS                               *
 *****************************************************************/
#define NUM_LINES 15

uint8_t running;
uint64_t *ret_ptr;
uint64_t *stack_ptr;
uint64_t *buf_ptr;

/******************************************************************
 *                           MACROS                               *
 *****************************************************************/
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

/* Prints a single 8 byte word to stderr:
 *
 * Ex:
 * 0x0007ffffffffe80c: 0x0f0e0d0c0b0a0908
 * 
 * win      - The window to which to draw the stack
 * line_ptr - The address to print
 * ypos     - The y position relative to win's origin to draw the lines.
 */
void
print_line(WINDOW *win, uint64_t *line_ptr, int ypos)
{
    mvwprintw(win, ypos + 2, 2, "%#018" PRIx64, *line_ptr);
    if (line_ptr == buf_ptr)
    {
        wprintw(win, " <- Buffer");
    }
    else if (line_ptr == stack_ptr)
    {
        wprintw(win, " <- %%RSP");
    }
    else if (line_ptr == ret_ptr)
    {
        wprintw(win, " <- Ret. Addr.");
    }
    wprintw(win, "\n");
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
void
print_stack(WINDOW *win)
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

/**
 * Rewritten c std library function that sleeps for 1 second after each write
 * to memory.
 */
char*
my_strcpy(char *dest, const char *src)
{
    GET_STACK_PTR();
    size_t i;

    // Copy contents of src into dest
    i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
        sleep(1);
    }
    // Insert the null character
    dest[i] = '\0';

    return dest;
}

/**
 * The nCurses thread run function.
 */
void*
cthread_run(void *arg)
{
    // Variables
    WINDOW *window_out; // Program output window
    WINDOW *window_src; // Program source window
    WINDOW *window_mem; // Program memory window

    PANEL *panel_out; // Program output panel
    PANEL *panel_src; // Program source panel
    PANEL *panel_mem; // Program memory panel
    
    // Setup nCurses
	initscr();
    curs_set(0);
    cbreak();
    noecho();

    // Window widths
    uint16_t w_src = COLS * 0.5 - 1;
    uint16_t w_mem = COLS * 0.5 - 1;
    uint16_t w_out = COLS - 2;

    // Window heights
    uint16_t h_src = LINES * 0.75 - 1;
    uint16_t h_mem = LINES * 0.75 - 1;
    uint16_t h_out = LINES * 0.25 - 1;

    // Window x positions
    uint16_t x_src = 1;
    uint16_t x_mem = w_src + 2;
    uint16_t x_out = 1;

    // Window y positions
    uint16_t y_src = 1;
    uint16_t y_mem = 1;
    uint16_t y_out = h_src + 1;

    // TODO: Setup inotify

    // Initializing windows and panels
    window_out = newwin(h_out, w_out, y_out, x_out);
    window_src = newwin(h_src, w_src, y_src, x_src);
    window_mem = newwin(h_mem, w_mem, y_mem, x_mem);
    panel_out = new_panel(window_out);
    panel_src = new_panel(window_src);
    panel_mem = new_panel(window_mem);

    // Check for and update bottom pannel
    // Update memory panel
    while (running) {
        print_stack(window_mem);

        box(window_out, 0, 0);
        box(window_src, 0, 0);
        box(window_mem, 0, 0);

        update_panels();
        doupdate();

        usleep(500000);
    }

    // Cleanup
    delwin(window_out);
    delwin(window_src);
    delwin(window_mem);
    del_panel(panel_out);
    del_panel(panel_src);
    del_panel(panel_mem);
    endwin();
    return NULL;
}

/**
 * Function that calls an unsafe subroutine.
 */
void
bad_func(void)
{
    char *hello = "0123456789012345";
    char buf[16];

    GET_BUF_PTR(buf);
    my_strcpy(buf, hello);
    return;
}

/**
 * Entry point for the program.
 */
int
main(int argc, char *argv[])
{
    // Defining globals
    running = 1;

    // Local variables
    pthread_t cthread;
    
    pthread_create(&cthread, NULL, cthread_run, NULL);

    // BEGIN main content of the program
    BEFORE_UNSAFE_CALL();
    bad_func();
    // END main content of the program
    
    running = 0;
    pthread_join(cthread, NULL);

	return EXIT_SUCCESS;	
}

