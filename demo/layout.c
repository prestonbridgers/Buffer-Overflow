#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <errno.h>
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
#define INOTIFY_MAX_EVENTS 1024 // iNotify event maximum
#define INOTIFY_NAME_LEN 32     // iNotify filename length maximum
#define INOTIFY_EVENT_SIZE \
    (sizeof(struct inotify_event))
#define INOTIFY_BUF_LEN \
    (INOTIFY_MAX_EVENTS * (INOTIFY_EVENT_SIZE + INOTIFY_NAME_LEN))

#define NUM_LINES 15

uint8_t running = 1;
uint64_t *ret_ptr = NULL;
uint64_t *stack_ptr = NULL;
uint64_t *buf_ptr = NULL;
FILE *fd_output;

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
    if (line_ptr == NULL) {
        return;
    }

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
    if (tmp == NULL) {
        return;
    }
    int i = 0;
    for (i = 0; i < NUM_LINES; i++)
    {
        print_line(win, tmp, i);
        tmp++;
    }
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

    fprintf(fd_output, "In my_strcpy()...\n");
    fflush(fd_output);

    fprintf(fd_output, "Buffer contents: ");
    fflush(fd_output);

    // Copy contents of src into dest
    i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];

        fprintf(fd_output, "%c", src[i]);
        fflush(fd_output);

        i++;
        sleep(1);
    }
    // Insert the null character
    dest[i] = '\0';

    fprintf(fd_output, "\n");
    fflush(fd_output);

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
    uint16_t w_src;
    if (COLS % 2) { // If the terminal width is odd, add 1 to src panel width
        w_src = COLS * 0.5 + 1;
    } else {
        w_src = COLS * 0.5;
    }

    uint16_t w_mem = COLS * 0.5 + 1;
    uint16_t w_out = COLS;

    // Window heights
    uint16_t h_src = LINES * 0.75;
    uint16_t h_mem = LINES * 0.75;
    uint16_t h_out = LINES * 0.25;

    // Window x positions
    uint16_t x_src = 0;
    uint16_t x_mem = w_src - 1;
    uint16_t x_out = 0;

    // Window y positions
    uint16_t y_src = 0;
    uint16_t y_mem = 0;
    uint16_t y_out = h_src - 1;

    // Setup inotify
    int inotify_fd, inotify_wd;
    char *inotify_filename = "prog.out";

    inotify_fd = inotify_init1(IN_NONBLOCK);
    
    // Adding inotify_filename to watch list
    inotify_wd = inotify_add_watch(inotify_fd, inotify_filename, IN_MODIFY);
    if (inotify_wd == -1) {
        fprintf(stderr, "Failed to Watch: %s\n", inotify_filename);
    } else {
        fprintf(stderr, "Watching: %s\n", inotify_filename);
    }

    // Initializing windows and panels
    window_out = newwin(h_out, w_out, y_out, x_out);
    window_src = newwin(h_src, w_src, y_src, x_src);
    window_mem = newwin(h_mem, w_mem, y_mem, x_mem);
    panel_src = new_panel(window_src);
    panel_mem = new_panel(window_mem);
    panel_out = new_panel(window_out);

    // Update memory panel
    while (running) {
        // Update output panel
        int i = 0;
        int length = 0;
        char buffer[INOTIFY_BUF_LEN];
        char c;
        long last_read = 0;
        int cursY = 1;
        int cursX = 1;

        length = read(inotify_fd, buffer, INOTIFY_BUF_LEN);
        while (i < length) {
            struct inotify_event *event = (struct inotify_event*) &buffer[i];

            if (event->mask & IN_MODIFY) {
                fprintf(stderr, "File %s was modified!\n", inotify_filename);
                // Seeking to the end of what we've read in the file so far
                fseek(fd_output, last_read, SEEK_SET);
                fprintf(stderr, "last_read: %ld\n", last_read);

                // Loop that reads the new lines of the file character by character
                c = fgetc(fd_output);
                while (c != EOF) {
                    if (c == '\n') {
                        cursY++;
                        cursX = 1;
                    }
                    else {
                        // Print the read character to the nCurses window
                        mvwaddch(window_out, cursY, cursX++, c);
                        fprintf(stderr, "Printed %c to the screen\n", c);
                    }
                    // Read the next character
                    c = fgetc(fd_output);
                    if (c == EOF) {
                        fprintf(stderr, "Reached the EOF character\n\n");
                    }
                }
            }
            i += INOTIFY_EVENT_SIZE + event->len;
            last_read = ftell(fd_output);
        }
        
        // Update memory panel
        print_stack(window_mem);

        box(window_out, 0, 0);
        box(window_src, 0, 0);
        box(window_mem, 0, 0);

        update_panels();
        doupdate();

        usleep(250000);
    }

    getch();
    // Cleanup
    del_panel(panel_out);
    del_panel(panel_src);
    del_panel(panel_mem);
    delwin(window_out);
    delwin(window_src);
    delwin(window_mem);
    endwin();
    return NULL;
}

/**
 * Function that calls an unsafe subroutine.
 */
void
bad_func(void)
{
    fprintf(fd_output, "In bad func...\n");
    fprintf(stderr, "Printed\n");
    fflush(fd_output);

    char *hello = "012345678901234";
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
    // Local variables
    pthread_t cthread;
   
    fd_output = fopen("prog.out", "w+");
    if (fd_output == NULL) {
        fprintf(stderr, "Failed to open file prog.out\n");
    }
    pthread_create(&cthread, NULL, cthread_run, NULL);
    sleep(1);

    // BEGIN main content of the program
    fprintf(fd_output, "Calling bad_func() ...\n");
    fprintf(stderr, "Printed\n");
    fflush(fd_output);

    BEFORE_UNSAFE_CALL();
    bad_func();

    fprintf(fd_output, "Returned from bad_func() ...\n");
    fprintf(stderr, "Printed\n");
    fflush(fd_output);
    // END main content of the program
    
    running = 0;
    pthread_join(cthread, NULL);
    fclose(fd_output);
	return EXIT_SUCCESS;	
}

